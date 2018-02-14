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
#include "../utils.h"
#include "../protocol.h"

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
        exit(EXIT_FAILURE);
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
    //print_response(response);
    return response;
}

int getkey() {
    printf("Press any key... ");
    return getchar();
}

char * get_movie(Response * response) {
    List movies = response_extract_movies(response);

    if (list_size(movies) == 0) {
        list_destroy(movies);
        return NULL;
    }

    char * movie;
    for (int i = 0; (movie = list_get_next(movies)) != NULL; i++){
        printf("%d- %s\n", i + 1, movie);
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

void print_showcase(Showcase * showcase, int index) {
    printf("%d- movie: %s, day: %d, room: %d\n", index, showcase->movie_name, showcase->day, showcase->room);
}

Showcase * get_showcase(Response * response) {
    List showcases = response_extract_showcases(response);

    if (list_size(showcases) == 0) {
        list_destroy(showcases);
        return NULL;
    }

    Showcase * aux;
    for (int i = 0; (aux = list_get_next(showcases)) != NULL; i++) {
        print_showcase(aux, i + 1);
    }

    int n;
    do {
        n = getint("Pick a showcase: ");
    } while (n <= 0 && n > list_size(showcases));

    aux = list_get(showcases, n - 1);
    Showcase * showcase = new_showcase(aux->movie_name, aux->day, aux->room);

    while ((aux = list_get_next(showcases)) != NULL) {
        destroy_showcase(aux);
    }

    list_destroy(showcases);
    return showcase;
}

int get_seat(Response * response) {
    int seats[SEATS];
    response_extract_seats(response, seats);

    int row, col;
    do {
        row = getint("Enter row: ");
        col = getint("Enter column: ");
    } while (row <= 0 || row > ROWS || col <= 0 || col > COLS);

    return row * col;
}

void buy_ticket(Client client, char * client_name) {
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

    Showcase * showcase  = get_showcase(response);
    destroy_response(response);

    if (showcase == NULL) {
        printf("There are no showcases for the movie: '%s'...\n", movie_name);
        free(movie_name);
        return;
    }

    // GET_SEATS
    send_request(client, GET_SEATS, "%s%d%d", showcase->movie_name, showcase->day, showcase->room);
    response = wait_response(client);

    int seat = get_seat(response);
    destroy_response(response);

    // ADD_BOOKING
    send_request(client, ADD_BOOKING, "%s%s%d%d%d", client_name, showcase->movie_name, showcase->day, showcase->room, seat);
    response = wait_response(client);

    if (response->status == RESPONSE_OK) {
        printf("Ticket bought!\n");
    } else {
        printf("Failed!\n");
    }

    destroy_response(response);
    destroy_showcase(showcase);
    free(movie_name);
}

char * get_day(int day) {
    char * ret;
    switch (day) {
        case SUN:
            ret = "SUNDAY";
            break;
        case MON:
            ret = "MONDAY";
            break;
        case TUE:
            ret = "TUESDAY";
            break;
        case WED:
            ret = "WEDNESDAY";
            break;
        case THU:
            ret = "THURSDAY";
            break;
        case FRI:
            ret = "FRIDAY";
            break;
        case SAT:
            ret = "SATURDAY";
            break;
        default:
            ret = "GG";
            break;
    }

    return ret;
}

void print_ticket(Ticket * ticket, char * client_name, int index) {
    printf("\t-Ticket %d-\n", index);
    printf("---------------------------\n");
    printf("\tName:  %s\n", client_name);
    printf("\tMovie: %s\n", ticket->showcase.movie_name);
    printf("\tDay:   %s\n", get_day(ticket->showcase.day));
    printf("\tRoom:  %d\n", ticket->showcase.room);
    printf("\tSeat:  %d\n", ticket->seat);
    printf("---------------------------\n");
}

void view_tickets(Client client, char * client_name) {
    // GET_BOOKING
    Response * response;
    send_request(client, GET_BOOKING, "%s", client_name);
    response = wait_response(client);

    List tickets = response_extract_tickets(response);
    Ticket * ticket;

    if (list_size(tickets) == 0) {
        printf("You don't have any tickets.\n");
    }

    for (int i = 0; (ticket = list_get_next(tickets)) != NULL; i++) {
        print_ticket(ticket, client_name, i+1);
        destroy_ticket(ticket);
    }

    destroy_response(response);
    list_destroy(tickets);

    // press key to go back
    getkey();
}

Ticket * get_ticket(Response * response, char * client_name) {
    List tickets = response_extract_tickets(response);

    if (list_size(tickets) == 0) {
        list_destroy(tickets);
        printf("You don't have any tickets.\n");
        return 0;
    }

    Ticket * aux;

    for (int i = 0; (aux = list_get_next(tickets)) != NULL; i++) {
        print_ticket(aux, client_name, i + 1);
    }

    int n;
    do {
        n = getint("Pick a ticket: ");
    } while (n <= 0 || n > list_size(tickets));

    aux = list_get(tickets, n - 1);

    Ticket * ticket = new_ticket(aux->showcase, aux->seat);

    for (int i = 0; (aux = list_get_next(tickets)) != NULL; i++) {
        destroy_ticket(aux);
    }
    list_destroy(tickets);

    return ticket;
}

void cancel_reservation(Client client, char * client_name) {
    // GET_BOOKING
    Response * response;
    send_request(client, GET_BOOKING, "%s", client_name);
    response = wait_response(client);

    Ticket * ticket = get_ticket(response, client_name);
    destroy_response(response);
    if (ticket == NULL) {
        return;
    }

    bool cancel = yesNo("Press (y/n): ");

    if (!cancel) {
        destroy_ticket(ticket);
        return;
    }

    // REMOVE_BOOKING
    send_request(client, REMOVE_BOOKING, "%s%s%d%d%d", client_name,
                 ticket->showcase.movie_name, ticket->showcase.day,
                 ticket->showcase.room, ticket->seat);
    response = wait_response(client);

    if (response->status == RESPONSE_OK) {
        printf("Reservation cancelled.\n");
    } else {
        printf("Error cancelling reservation.\n");
    }
    destroy_response(response);
    destroy_ticket(ticket);
}

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

    Showcase * showcase = get_showcase(response);
    destroy_response(response);

    if (showcase == NULL) {
        printf("There are no showcases for the movie: '%s'...\n", movie_name);
        free(movie_name);
        return;
    }

    // REMOVE_SHOWCASE
    send_request(client, REMOVE_SHOWCASE, "%s%d%d", showcase->movie_name, showcase->day, showcase->room);
    response = wait_response(client);

    if (response->status == RESPONSE_OK) {
        printf("Showcase removed..\n");
    } else {
        printf("Error removing showcase.\n");
    }

    destroy_response(response);
    destroy_showcase(showcase);
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

    if (status == RESPONSE_OK) {
        printf("Welcome %s!\n\n", client_name);
    } else if (status == ALREADY_EXIST){
        printf("Welcome back %s!\n\n", client_name);
    } else {
        printf("Login error\n");
        exit(EXIT_FAILURE);
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
