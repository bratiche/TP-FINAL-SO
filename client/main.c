/**
 * Frontend
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "getnum.h"
#include "client.h"

#define CLEAR_BUFFER        while (getchar() != '\n')
#define CLIENT_NAME_LENGTH  32

typedef enum {
    BUY = 1,
    VIEW,
    CANCEL,
    EXIT
} main_menu_option;

int get_string(char * msg, char * buff, unsigned int max_len);
int get_option(char * msg, char ** options, unsigned int count);

int getkey() {
    printf("Press any key... ");
    return getchar();
}

void buy_ticket(Client client) {
    //TODO query
    char * movies[] = {"EZ", "PZ", "Lemon", "Squeeze"};
    unsigned int count = 4;

    // pick movie
    int movie = get_option("Choose a movie: ", movies, count);

    // pick showcase

    // pick seat
}

void view_tickets(Client client) {
    //TODO query

    // show tickets
    //Ticket * tickets;

    printf("Ticket 1: EZ\n");
    printf("Ticket 2: PZ\n\n");

    //press key to go back
    getkey();
}

void cancel_reservation(Client client) {
    //TODO query
    char * reservations[] = {"EZ", "PZ"};
    unsigned int count = 2;

    // pick ticket
    int ticket = get_option("Choose a reservation to cancel: ", reservations, count);

    // ask confirmation
}

#define BUFFER_SIZE 4098

void client_start(Client client) {
//    char buff[CLIENT_NAME_LENGHT + 1] = {0};
//    int ret = 0;
//    do {
//        ret = get_string("Enter your name: ", buff, CLIENT_NAME_LENGHT + 1);
//    } while (ret == 0);
//
//    printf("Welcome %s!\n\n", buff);
//

    char * msgs[] = {"Buy a ticket", "View tickets", "Cancel a reservation", "Exit"};

    char buffer[BUFFER_SIZE];
    while (true) {
        main_menu_option option;
        //main_menu_option option = (main_menu_option) get_option("Choose an option: ", msgs, EXIT);

        //todo remove
        fgets(buffer, BUFFER_SIZE, stdin);
        client_send(client, buffer);

        if (client_recv(client, buffer) <= 0) {
            printf("Server closed connection!\n");
            return;
        }
        printf("%s", buffer);
        //

        switch (option) {
            case BUY:
                //buy_ticket(client);
                break;
            case VIEW:
                //view_tickets(client);
                break;
            case CANCEL:
                //cancel_reservation(client);
                break;
            case EXIT:
                return;
        }
    }
}

int main(int argc, char*argv[]) {

    char * hostname = "localhost";
    int server_port = 12345;

    Client client = new_client(hostname, server_port);

    if (client == NULL) {
        return -1;
    }

    client_start(client);
    close_client(client);
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
