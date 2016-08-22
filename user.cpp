#include <iostream>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc!=6)
    {
        
        cout<<"please give 5 arguments"<<endl;
        exit(4);
    }
    int sockfd;//will hold the socket file discriptor of server
    struct sockaddr_in dest_addr;// will hold the destination ip addr i.e of server
    sockfd = socket(PF_INET, SOCK_STREAM, 0);//creating socket
    if (sockfd==-1)
    {
        cout<<"socket could not be created"<<endl;// problem creating socket
        exit(4);
    }
    cout<<"socket created"<<endl;//socket created
    dest_addr.sin_family = AF_INET;// host byte order
    dest_addr.sin_port = htons(atoi(argv[2]));   // short, network byte order
    dest_addr.sin_addr.s_addr = inet_addr(argv[1]); // Destination adress is set to input given
    memset(&(dest_addr.sin_zero), '\0', 8);  // zero the rest of the struct
    int error=connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));//connecting to server
    if (error==-1)
    {
        cout<<"connection failed"<<endl;
        exit(4);
    }
    cout<<"connected"<<endl;
    string data=argv[3];
    data=data+" ";
    data=data+argv[4];
    data=data+" ";
    data=data+argv[5];
    char s[300];
    bzero(s, 301);
    s[0]='1';
    //clock_t t1,t2;
    //t1=clock();
    int bytes_sent = send(sockfd,s,strlen(s),0);// this messege is sent to say that i am client
    for (int i = 0; i < data.length(); ++i)
    {
        s[i]=data[i];
    }
    bytes_sent = send(sockfd,s,strlen(s),0);// sending hash and 
    char recived_password[300];
    bzero(recived_password, 301);
    int bytes_recived = recv(sockfd,recived_password,300,0);
    if(bytes_recived==-1)
    {
        cout<<"there was an error in recieving"<<endl;
    }
    cout<<"password recieved and password is "<<recived_password<<endl;
    //t2=clock();
    //float diff((float)t2-(float)t1);
    //cout<<"time taken is"<<diff/CLOCKS_PER_SEC<<endl;
    return 0; 
}