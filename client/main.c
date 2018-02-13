/**
 * Frontend
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getnum.h"
#include "client.h"
#include "response_parser.h"
#include "../common/utils.h"
#include "../common/protocol.h"

#define CLEAR_BUFFER        while (getchar() != '\n')

typedef enum {
    LOGIN_ADMIN = 1,
    LOGIN_CLIENT,
    LOGIN_EXIT
} login_menu_option;

typedef enum {
    ADMIN_ADD_SHOWCASE = 1,
    ADMIN_REMOVE_SHOWCASE,

    ADMIN_EXIT,
} admin_menu_option;

typedef enum {
    CLIENT_BUY_TICKET = 1,
    CLIENT_VIEW_TICKETS,
    CLIENT_CANCEL_RESERVATION,
    CLIENT_EXIT
} client_menu_option;

int get_string(char * msg, char * buff, unsigned int max_len);
int get_option(char * msg, char ** options, unsigned int count);

/** Serializes a request and sends it to the server */
ssize_t send_request(Client client, int request_type, const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    char buffer[BUFFER_SIZE] = {0};
    char * aux = buffer;

    aux += sprintf(aux, "%d\n", request_type);
    while(*fmt != 0) {
        char c = *fmt++;
        if (c == '%') {
            c = *fmt++;
            switch (c) {
                case 's':
                    aux += sprintf(aux, "%s\n", va_arg(ap, char *));
                    break;
                case 'd':
                    aux += sprintf(aux, "%d\n", va_arg(ap, int));
                    break;
                default:
                    break;
            }
        }
    }

    va_end(ap);
    sprintf(aux, ".\n");

    //fprintf(stderr, "'%s'", buffer);

    return client_send(client, buffer);
}

/** Waits until the server response is finished */
Response * wait_response(Client client) {
    Response * response = new_response();
    if (response == NULL) {
        fprintf(stderr, "Memory error");
        exit(EXIT_FAILURE); //GG
    }

    char buffer[BUFFER_SIZE] = {0};
    ResponseParser parser;
    response_parser_init(&parser, response);
    bool done;

    printf("Waiting server response...\n");
    do {
        //bzero(buffer, BUFFER_SIZE);
        client_recv(client, buffer);
        response_parser_consume(&parser, buffer);
        done = response_parser_is_done(&parser, 0);
    } while(!done);

    response_parser_destroy(&parser);

    printf("Server response: %s\n", get_response_status(response));
    return response;
}

int getkey() {
    printf("Press any key... ");
    return getchar();
}

int get_showcases(char * movie) {
    return 0;
}

int get_seat() {
    return 0;
}

int book(char * client, char * movie, int day, int room, int seat) {

}


void buy_ticket(Client client, char * client_name) {
    //TODO query GET_MOVIES
    char * movies[] = {"EZ", "PZ", "Lemon", "Squeeze"};
    unsigned int count = 4;

    // pick movie
    int movie = get_option("Choose a movie: ", movies, count);

    // pick showcase
    // TODO query GET_SHOWCASES
    int showcase = get_showcases(movies[movie]);

    // pick seat
    // TODO query GET_SEATS
    int seat = get_seat();

    // TODO query ADD_BOOKING
    int booking = book(client_name, movies[movie], 0, 0, seat);

    if (booking != 0) {
        //ERROR
    }
}

void view_tickets(Client client, char * client_name) {
    //TODO query GET_RESERVATIONS

    // show tickets
    //Ticket * tickets;

    printf("Ticket 1: EZ\n");
    printf("Ticket 2: PZ\n\n");

    //press key to go back
    getkey();
}

void cancel_reservation(Client client, char * client_name) {
    //TODO query
    char * reservations[] = {"EZ", "PZ"};
    unsigned int count = 2;

    // pick ticket
    int ticket = get_option("Choose a reservation to cancel: ", reservations, count);

    // ask confirmation
}

#define MOVIE_NAME_LENGTH 50
#define ROOMS             5

void admin_add_showcase(Client client) {

    char movie_name[MOVIE_NAME_LENGTH];

    get_string("Enter movie name: ", movie_name, MOVIE_NAME_LENGTH);

    char * days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    int day = get_option("Pick a day: ", days, 7);
    int room;

    do {
        room = getint("Pick a room (between 1 and 5): ");
        if (room <= 0 || room > ROOMS) {
            printf("Invalid room number");
        }
    } while (room <= 0 || room > ROOMS);


    send_request(client, ADD_SHOWCASE, "%s%d%d", movie_name, day, room);
    Response *response = wait_response(client);
    if (response->status == RESPONSE_OK) {
        printf("Showcase added..\n");
    } else {
        printf("Error adding showcase.\n");
    }
    destroy_response(response);
}

char * get_movie(Response * response) {
    List movies = response_extract_movies(response);

    if (list_size(movies) == 0) {
        list_destroy(movies);
        return NULL;
    }

    char * movie;
    for (int i = 0; (movie = list_get_next(movies)) != NULL; i++){
        printf("%d- %s", i + 1, movie);
    }

    int n;
    do {
        n = getint("Pick a movie: ");
    } while (n <= 0 && n > list_size(movies));

    movie = list_get(movies, n - 1);
    char * ret =  calloc(strlen(movie) + 1, sizeof(char));
    if (ret == NULL) {
        fprintf(stderr, "Memory error.");
        exit(EXIT_FAILURE);
    }
    strcpy(ret, movie);
    list_destroy(movies);
    return ret;
}

int get_showcase(Response * response, Showcase * showcase) {
    List showcases = response_extract_showcases(response);

    if (list_size(showcases) == 0) {
        list_destroy(showcases);
        return -1;
    }

    Showcase * aux;
    for (int i = 0; (aux = list_get_next(showcases)) != NULL; i++){
        printf("%d- movie: %s, day: %d, room: %d", i + 1, aux->movie_name, aux->day, aux->room);
    }

    int n;
    do {
        n = getint("Pick a showcase: ");
    } while (n < 0 && n > list_size(showcases));

    aux = list_get(showcases, n);
    showcase->day = aux->day;
    showcase->room = aux->room;

    while ((aux = list_get_next(showcases)) != NULL) {
        free(aux);
    }

    list_destroy(showcases);
    return 0;
}

void admin_remove_showcase(Client client) {
    // GET_MOVIES
    Response * response;
    send_request(client, GET_MOVIES, "");
    response = wait_response(client);

    char * movie_name = get_movie(response);
    destroy_response(response);

    if (movie_name == NULL) {
        printf("There are no movies...\n");
        return;
    }

    // GET_SHOWCASES
    send_request(client, GET_SHOWCASES, "%s", movie_name);
    response = wait_response(client);

    Showcase showcase;
    showcase.movie_name = movie_name;
    int ret = get_showcase(response, &showcase);
    destroy_response(response);

    if (ret != 0) {
        printf("There are no showcases for the movie: '%s'...\n", movie_name);
        return;
    }
    // REMOVE_SHOWCASE
    send_request(client, REMOVE_SHOWCASE, "%s%d%d", movie_name, showcase.day, showcase.room);
    response = wait_response(client);

    if (response->status == RESPONSE_OK) {
        printf("Showcase removed..\n");
    } else {
        printf("Error removing showcase.\n");
    }

    destroy_response(response);
    free(movie_name);
}


/** Admin options */
void admin_menu(Client client) {

    char * options[] = {"Add showcase", "Remove showcase", "Exit"};

    printf("Logged in as Administrator.\n\n");

    while (true) {
        admin_menu_option option = (admin_menu_option) get_option("Choose an option: ", options, ADMIN_EXIT);

        switch (option) {
            case ADMIN_ADD_SHOWCASE:
                admin_add_showcase(client);
                break;
            case ADMIN_REMOVE_SHOWCASE:
                admin_remove_showcase(client);
                break;
            case ADMIN_EXIT:
                return;
        }
    }
}

void add_new_client(Client client, char * client_name) {
    send_request(client, ADD_CLIENT, "%s", client_name);
    Response * response = wait_response(client);
    int status = response->status;
    destroy_response(response);

    if (status != RESPONSE_OK) {
        printf("Login error\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Welcome %s!\n\n", client_name);
    }
}

/** Client options */
void client_menu(Client client) {

    char client_name[CLIENT_NAME_LENGTH + 1] = {0};
    int ret = 0;
    do {
        ret = get_string("Enter your name: ", client_name, CLIENT_NAME_LENGTH + 1);
    } while (ret == 0);

    add_new_client(client, client_name);

    char * options[] = {"Buy a ticket", "View tickets", "Cancel a reservation", "Exit"};

    while (true) {
        client_menu_option option =  (client_menu_option) get_option("Choose an option: ", options, CLIENT_EXIT);
        switch (option) {
            case CLIENT_BUY_TICKET:
                buy_ticket(client, client_name);
                break;
            case CLIENT_VIEW_TICKETS:
                view_tickets(client, client_name);
                break;
            case CLIENT_CANCEL_RESERVATION:
                cancel_reservation(client, client_name);
                break;
            case CLIENT_EXIT:
                return;
        }
    }
}

/** Login */
void client_start(Client client) {
    char * options[] = {"Login as Admin", "Login as client", "Exit"};

    while (true) {
        login_menu_option option = (login_menu_option) get_option("Login as: ", options, LOGIN_EXIT);

        switch (option) {
            case LOGIN_ADMIN:
                admin_menu(client);
                break;
            case LOGIN_CLIENT:
                client_menu(client);
                break;
            case LOGIN_EXIT:
                return;
        }
    }
}

int main(int argc, char*argv[]) {

    char * hostname = DEFAULT_HOST;
    int server_port = DEFAULT_PORT;
    if (argc >= 2) {
        hostname = argv[1];
        if (argc >=3) {
            server_port = parse_port(argv[2]);
        }
    }

    Client client = client_init(hostname, server_port);
    if (client == NULL) {
        return -1;
    }

    client_start(client);
    client_close(client);
    return 0;
}

int get_string(char * msg, char * buff, unsigned int max_len) {
    printf("%s",msg);
    int c;
    int i = 0;
    while((c=getchar())!='\n' && i < max_len){
        buff[i] = (char) c;
        i++;
    }
    buff[i]=0;

    if(c != '\n'){
        CLEAR_BUFFER;
    }

    return i;
}

int get_option(char * msg, char ** options, unsigned int count) {
    int option = 0;

    do {
        for (int i = 0; i < count; i++) {
            printf("%d- %s\n", i + 1, options[i]);
        }
        option = getint(msg);
    } while(option < 0 || option > count);

    return option;
}
