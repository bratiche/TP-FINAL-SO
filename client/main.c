/**
 * Frontend
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "getnum.h"
#include "client.h"
#include "../common/utils.h"

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

int getkey() {
    printf("Press any key... ");
    return getchar();
}

int get_showcase(char * movie) {
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
    int showcase = get_showcase(movies[movie]);

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

/** Admin options */
void admin_menu(Client client) {

    char * options[] = {"Add showcase", "Remove showcase", "Exit"};

    printf("Welcome admin!\n\n");

    while (true) {
        admin_menu_option option = (admin_menu_option) get_option("Choose an option: ", options, ADMIN_EXIT);

        switch (option) {
            case ADMIN_ADD_SHOWCASE:
                break;
            case ADMIN_REMOVE_SHOWCASE:
                break;
            case ADMIN_EXIT:
                return;
        }
    }
}

/** Client options */
void client_menu(Client client) {

    char client_name[CLIENT_NAME_LENGTH + 1] = {0};
    int ret = 0;
    do {
        ret = get_string("Enter your name: ", client_name, CLIENT_NAME_LENGTH + 1);
    } while (ret == 0);

    printf("Welcome %s!\n\n", client_name);

    //TODO QUERY ADD_CLIENT(buff)

    char * options[] = {"Buy a ticket", "View tickets", "Cancel a reservation", "Exit"};

    while (true) {
        client_menu_option option =  (client_menu_option) get_option("Choose an option: ", options, CLIENT_EXIT);

//        char buffer[BUFFER_SIZE];
//        fgets(buffer, BUFFER_SIZE, stdin);
//        client_send(client, buffer);
//
//        if (client_recv(client, buffer) <= 0) {
//            printf("Server closed connection!\n");
//            return;
//        }
//        printf("%s", buffer);

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
