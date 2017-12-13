#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE   1
#define FALSE  0
#define PORT 3000
#define MAXLINE 1025 /*max text line length*/


typedef struct User
{
  char * name;
  int socket;
  int state;// 1 : free 0: playing
}user;

typedef struct play_table
{
  user user1;
  user user2;
  int point1;//điểm của user 1
  int point2;// điểm của user 2
}play_table;

typedef struct user_list
{
  user us;
  struct user_list *next;
}list;

list * root;

list * addlist(user acc)
{
    list *cur;
    if(root == NULL)
        {root = malloc(sizeof(list));root->us=acc;root->next=NULL;}
    else
      {
        cur = root;
        while (cur->next!=NULL)
        {
            cur = cur->next;
        }
        cur->next = malloc(sizeof(list));
        cur->next->us =acc;
        cur->next->next = NULL;
      }

      return root;

}

int search_sock(char *name,list *demo)
{
    list * cur;
    cur = malloc(sizeof(list));
    if(demo != NULL)
    {
    cur = demo;
    while(cur!=NULL)
    {
        if(strcmp(cur->us.name,name)==0) return cur->us.socket;
        cur = cur->next;
    }
    }
    return -1;
}

user search_user(int sock,list *demo)
{
    list * cur;
    cur = malloc(sizeof(list));
    if(demo != NULL)
    {
    cur = demo;
    while(cur!=NULL)
    {
        if(cur->us.socket == sock )return cur->us;
        cur = cur->next;
    }
    }
}

user make_user(char *str,int sock)// tao user moi
{
    user new ;
    new.name = malloc(30*sizeof(char));
    strcpy(new.name,str);
    new.socket = sock;
    new.state = 1 ;
    return new;
}

// Kiem tra ten da ton tai trong list hay chua
// da ton tai : 0    chua ton tai tra ve : 1
int checkname(char * name, list * user_list)
{
    list * cur;
    cur = malloc(sizeof(list));
    if(user_list != NULL)
    {
    cur = user_list;
    while(cur!=NULL)
    {
        if(name != NULL)
        {
        if(strcmp(cur->us.name,name)== 0 )return 0;
        }else printf("name is null :checkname \n");
        cur = cur->next;
    }
    }
    else printf("danh sach rong: checkname\n");

    return 1;
}

char * getlistuser(list *demo) // lay danh sach ten user
{
    list * cur;
    cur = malloc(sizeof(list));
    char* list_user = malloc(256*sizeof(char));
    if(demo!=NULL)
    {
    strcpy(list_user,demo->us.name);
    cur = demo->next;
    while(cur!=NULL)
    {
        if(cur->us.state == 1)
        {
            sprintf(list_user,"%s %s",list_user,cur->us.name);
        }
        cur = cur->next;
    }
    return list_user;
    }
    else return NULL;
}

void play(user user1, user user2)// xu ly tro choi
{

}

void xulymess(char * mess,int sock)
{
    char *code = malloc(5*sizeof(char));
    char *info = malloc(5*sizeof(char));
    char *sendline = malloc(256*sizeof(char));
    char *list_user = malloc(250*sizeof(char));

    sscanf(mess,"%s %s",code,info);
    puts(mess);
    switch(atoi(code))
    {
        case 101:
        {
            list_user = getlistuser(root);
            puts(list_user);
            sprintf(sendline,"201 %s",list_user);
            puts(sendline);
            send(sock , sendline , strlen(sendline) , 0 );
            break;
        }
        case 102:
             {
                int enemy;
                enemy = search_sock(info,root);
                if(enemy<0)puts("loi day roi");
                strcpy(list_user,search_user(sock,root).name);
                puts(list_user);
                sprintf(sendline,"203 %s",list_user);
                puts(sendline);
                send(enemy , sendline , strlen(sendline) , 0 );
                break;
             }
        case 104:break;
        case 105:break;
        case 106:
            {
                sendline ="209";
                int enemy;
                enemy = search_sock(info,root);
                if(enemy<0)puts("loi day roi 2");
                send(enemy , sendline , strlen(sendline) , 0 );
                send(sock , sendline , strlen(sendline) , 0 );
            break;
            }
        case 107:
        {
            if(checkname(info,root)== 1 )
            {
                user acc = make_user(info,sock);
                root = addlist(acc);
                strcpy(sendline,"207");
                send(sock , sendline , strlen(sendline) , 0 );
            }
            else
            {
                strcpy(sendline,"206");
                send(sock , sendline , strlen(sendline) , 0 );

            }
            break;
        }
        default :
        break;
    }
}

int main(int argc , char *argv[])
{
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[30] ,
     max_clients = 30 , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025];  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;

    //a message
    char *message = "ECHO Daemon v1.0 \r\n";

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(TRUE)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            // // //send new connection greeting message
            // if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            // {
            //     perror("send");
            // }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
                }
            }
        }

        //else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }

                //Echo back the message that came in
                else
                {
                    int n;
                    // //set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    // send(sd , buffer , strlen(buffer) , 0 );
                    xulymess(buffer,sd);
                }
            }
        }
    }

    return 0;
}
