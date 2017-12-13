#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 1025 /*max text line length*/
#define SERV_PORT 3000 /*port*/

struct Users {
  char name[30];
  int status; //0: WAIT, 1: FREE, 2: PLAYING
};
struct Game {
  int status;
  int otherPoint;
  int point;
  int winRound;
};

//function doc recvline
int ReadHeader (char* recvline) {
  int head;
  head = atoi(recvline);
  return head;
}

//chon user bang ID i trong userlist
void pickUserByID(char* userlist[100], int length, int sockfd) {
  char *sendline= malloc(MAXLINE*sizeof(char));
  int pick;
  printf("Pick user: "); scanf("%d", &pick);
  for (int j = 1; j < length; j++) {
    if (pick == j) {
      printf("Sent request to user %s\n", userlist[j]);
      //gui loi moi choi len server
      sprintf(sendline, "102 %s", userlist[j]);
      send(sockfd, sendline, strlen(sendline) + 1, 0);
    }
  }
}

//gui lenh len server lay danh sach
void getList(int sockfd, char *userlist[100]) {
  char sendline[MAXLINE];
  char* token;
  char * recvline = malloc(MAXLINE*sizeof(char));
  int i = 0;
  strcpy(sendline, "101");
  send(sockfd, sendline, strlen(sendline) + 1, 0);
  printf("Dang lay danh sach nguoi choi\n");
  if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
    //error: server terminated prematurely
    perror("The server terminated prematurely");
    exit(4);
  }
  else if (ReadHeader(recvline) == 201) {
    // printf("User List: %s\n", recvline);
    //in ra danh sach nguoi choi ranh
    for (token = strtok(recvline, " "); token != NULL; token = strtok(NULL, " ")) {
      userlist[i] = malloc(30 * sizeof(char));
      strcpy(userlist[i], token);
      if (i != 0)
        printf("User: %s\n", userlist[i]);
      i++;
    }
  }
    pickUserByID(userlist, i, sockfd); //chon nguoi choi

  sendline[0] = '\0';
}

//vao choi
void Playing (int sockfd) {
  // char sendline[MAXLINE], recvline[MAXLINE];
  // int value, i = 1;
  // printf("Start\n");
  // while (i <= 9) {
  //   printf("Round %d\n", i);
  //   if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
  //     //error: server terminated prematurely
  //     perror("The server terminated prematurely");
  //     exit(4);
  //   }
  //   else if (ReadHeader(recvline) == 208) {
  //     printf("Enter a number (0 - 99) you want to play in this round\n"); scanf("%d", &value);

  //     sprintf(sendline, "104 %d", value);
  //   }
  //   i++;
  // }
  printf("playing\n");
}

// khi nhan duoc yeu cau choi
//if ReadHeader = 203
void receivedRequest (int sockfd, char *recvline) {
  char *token= malloc(MAXLINE*sizeof(char));
  char *sendline= malloc(MAXLINE*sizeof(char));
  int ans;
  sscanf(recvline,"203 %s",token);
  printf("Received a request form user %s\n", token);
  printf("Accept (1) or not (0)? "); scanf("%d", &ans);
  if (ans == 1) {
    sprintf(sendline, "106 %s", token);  //gui chap nhan loi moi cua doi phuong len server
    send(sockfd, sendline, strlen(sendline) + 1, 0);
    if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
      //error: server terminated prematurely
      perror("The server terminated prematurely");
      exit(4);
    }
    if (ReadHeader(recvline) == 209) {
        printf("playing\n");
      Playing(sockfd);
    }
  }
  else {
    printf("Canceled\n");
  }
}

//nhap ten
char* enterName(int sockfd) {
  char sendline[MAXLINE], recvline[MAXLINE];
  char * name = malloc(MAXLINE*sizeof(char));
  printf("Please enter your name: ");
  while (1) {
    if (fgets(name, MAXLINE, stdin) != NULL) {
      sprintf(sendline, "107 %s", name);
    }
    send(sockfd, sendline, strlen(sendline) + 1, 0);
    int n = recv(sockfd, recvline, MAXLINE, 0);
    if ( n == 0) {
      //error: server terminated prematurely
      perror("The server terminated prematurely");
      exit(4);
    }
    else if (ReadHeader(recvline) == 206) {
      printf("Your name is used. Please enter another name\n");
      printf("Please enter your name: ");
    }
    //name okie roi
    else if (ReadHeader(recvline) == 207) {
      break;
    }
  }
  return name;
}

  int main(int argc, char **argv)
  {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];
    char * recvline = malloc(MAXLINE*sizeof(char));
    char* userlist[100];

    struct Users user1;
    user1.status = 0;

//basic check of the arguments
//additional checks can be inserted
    if (argc != 2) {
      perror("Usage: TCPClient <IP address of the server");
      exit(1);
    }

//Create a socket for the client
//If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Problem in creating the socket");
      exit(2);
    }

//Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order

//Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
      perror("Problem in connecting to the server");
      exit(3);
    }
//nhap ten
    // if (!user1.status) {
    //   strcpy(user1.name, enterName(sockfd));
    //   user1.status = 1;
    // }
    // printf("Welcome user %s\n", user1.name);
    // getList(sockfd, userlist);
    // if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
    //   //error: server terminated prematurely
    //   perror("The server terminated prematurely");
    //   exit(4);
    // } else if (ReadHeader(recvline) == 203) receivedRequest(sockfd, recvline);
    // exit(0);
    while(1)
    {
      int x;
      printf("1: Dang ky ten dang nhap \n2: thoat\n ");
      scanf("%d",&x);
      while(getchar()!='\n')continue;
      switch(x)
      {
         case 1:
         {
          if (!user1.status) {
          strcpy(user1.name, enterName(sockfd));
          user1.status = 1;
          }
          printf("Welcome user %s\n", user1.name);
          int y;
          printf("1: Lay danh sach \n2: thoat\n ");
          scanf("%d",&y);
          switch(y)
          {
            case 1:
            {
              getList(sockfd, userlist);
              if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
                //error: server terminated prematurely
                perror("The server terminated prematurely");
                exit(4);
              } else if (ReadHeader(recvline) == 203) receivedRequest(sockfd, recvline);
            }
            case 2:break;
          }
         break;
         }
         case 2:break;
         default:break;
      } exit(0);
    }
  }
