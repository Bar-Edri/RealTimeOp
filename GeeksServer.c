#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>
#define MAX 10
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.
void Sounds(char *buff){
	char a[]="aplay -d 1 Simon_";
	char b[]=".wav";
	char str[80]="\0";
	printf("Entering Sound Func %s \n",buff);
	for(int i=0;i<strlen(buff);i++){
		int k=0,j=0;
		while(a[k]!='\0'){
			str[k]=a[k];
			k++;
		}
		str[k]=buff[i];
		k++;
		for(int i=0;i<strlen(buff);i++){
			while(b[j]!='\0'){
				str[k]=b[j];
				j++;
				k++;
			}
		}
		system(str);
		bzero(str,80);
	}
}
void rand_str(char* dest,size_t length){
	char charset[]="RGBY";
	srand(time(NULL));
	while(length-->0){
	size_t index=(double)rand()/RAND_MAX * (sizeof charset-1);
	*dest++ = charset[index];
	}
	*dest='\0';
}


void func(int connfd)
{
    char buff[10],ServerMsg[10];
    int ClientLen=0;
    int exiting=1;
    int n=0;
    int SeqNum=1;
    int status=0;
    bzero(ServerMsg,10);
    // infinite loop for chat
    for (;;) {
        bzero(buff, 10);
        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        if(status==0){
		printf("From client: %s\t To client : ", buff);
		status++;
	}
	if((strncmp(buff,"Y",1)==0)&&(status==0)){
		printf("Client wants to play\n");
		status=1;
		bzero(buff,10);
	}

	if((strncmp(buff,"N",1)==0)){
		write(connfd,"OK GoodBye\n",sizeof(10));
		break;
	}

	if(status==1){
		printf("status 1\n");
		bzero(buff,10);
		rand_str(buff,SeqNum);
		assert(buff[9]=='\0');
		printf("This is the STRING %s\n",buff);
		write(connfd,buff,sizeof(buff));
		strcpy(ServerMsg,buff);
		status++;
		SeqNum++;
		bzero(buff,10);
	}
	if(status==2 && (strcmp(buff,"")!=0)){
		printf("Status -2\n");
		printf("Server - %s\nClient - %s",ServerMsg,buff);
		printf("return %d\n",strcmp(ServerMsg,buff));
		if(strcmp(ServerMsg,buff)==-10){
			printf("Entered\n");
			rand_str(buff,SeqNum);
			assert(buff[9]=='\0');
			Sounds(buff);
			printf("Correct, next - %s\n",buff);
			write(connfd,buff,sizeof(buff));
			strcpy(ServerMsg,buff);
			SeqNum++;
		}
		else{
			printf("Incorrect\n");
			break;
		}
		if(SeqNum>8){
			printf("GoodJob You have finished the game\n");
			break;
		}
	}
	bzero(buff,10);

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0 && status>0) {
            printf("Server Exit...\n");
	    write(connfd,buff,sizeof(buff));
            break;
        }
    }
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    // Function for chatting between client and server
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
}
