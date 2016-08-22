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
    if (argc!=3)
    {

        cout<<"please give 2 arguments"<<endl;
        exit(4);
    }
    int sockfd;//will hold the socket file discriptor of server
    struct sockaddr_in dest_addr;// will hold the destination ip addr i.e of server
    dest_addr.sin_family = AF_INET;// host byte order
    dest_addr.sin_port = htons(atoi(argv[2]));   // short, network byte order
    dest_addr.sin_addr.s_addr = inet_addr(argv[1]); // Destination adress is set to input given
    memset(&(dest_addr.sin_zero), '\0', 8);  // zero the rest of the struct
    bool connNotActive=true;//to indicate whether connected or not

    while(true)
    {
        if (connNotActive)
        {
            sockfd = socket(PF_INET, SOCK_STREAM, 0);//creating socket
            if (sockfd==-1)
            {
                cout<<"socket could not be created"<<endl;// problem creating socket
                exit(4);
            }
            cout<<"socket created"<<endl;//socket created


            int error=connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));//connecting to server
            if (error==-1)
            {
                cout<<"connection failed"<<endl;
                exit(4);
            }
            cout<<"connected"<<endl;
            char sayingWorker[300];
            bzero(sayingWorker, 301);
            sayingWorker[0]='0';
            int bytes_sent = send(sockfd,sayingWorker,strlen(sayingWorker),0);// this messege is sent to say that i am client
            connNotActive=false;
        }
        char work[300];
        bzero(work, 301);
        int recievedDataLength = recv(sockfd,work,300,0);//recieving the work
        work[recievedDataLength]='\0';
        if(recievedDataLength==-1)
        {
            cout<<"exceptionally closed"<<endl;
        }
        cout<<"recieved"<<endl;
        string intial="",finish="",type="",hash="";//intializing all the strings required
        int index=0;
        for (; work[index]!=' '; ++index)
        {
            intial+=work[index];
        }
        for (index++; work[index]!=' '; ++index)
        {
            finish+=work[index];
        }
        for (index++; work[index]!=' '; ++index)
        {
            type+=work[index];
        }
        for (index++; index<recievedDataLength && work[index]!=' '; ++index)
        {
            hash+=work[index];
        }
        //gathered all the information from buffer recieved
        string i = intial;//here i is iterator
        int length=intial.length();//length of password
        string salt = "";
        salt+=hash[0];
        salt+=hash[1];//found salt for crypt
        cout<<i<<" "<<finish<<" "<<hash<<" "<<length<<" "<<salt<<" "<<type<<endl;//just printing all required
        for(int count=0; i!=finish; count++)
        {
            string present=crypt(i.c_str(),salt.c_str()); // stored in a temp variable to compare
            if(present==hash)
            {
                int bytes_sent = send(sockfd,i.c_str(),i.length(),0);
                break;
            }
            if(type=="111")
            {
                for(int j=length-1; j>=0; j--)
                {
                    if(i[j]>=48 && i[j]<57)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else if(i[j] == 57)
                    {
                        i[j]='A';
                        break;
                    }
                    else if(i[j]>=65 && i[j]<90)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else if(i[j] == 90)
                    {
                        i[j] = 'a';
                        break;
                    }
                    else if(i[j]>=97 && i[j]<122)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else
                    {
                        i[j] = '0';
                    }
                }
            }// incremented last one if last one reaches maximum moved to previous one and then incremented that till it reaches the finish
            else if(type=="110")
            {
                for(int j=length-1; j>=0; j--)
                {
                    if(i[j]>=65 && i[j]<90)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else if(i[j] == 90)
                    {
                        i[j] = 'a';
                        break;
                    }
                    else if(i[j]>=97 && i[j]<122)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else
                    {
                        i[j] = 'A';
                    }
                }
            }
            else if(type=="101")
            {
                for(int j=length-1; j>=0; j--)
                {
                    if(i[j]>=48 && i[j]<57)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else if(i[j] == 57)
                    {
                        i[j]='a';
                        break;
                    }
                    else if(i[j]>=97 && i[j]<122)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else
                    {
                        i[j] = '0';
                    }
                }
            }
            else if(type=="100")
            {
                for(int j=length-1; j>=0; j--)
                {
                    if(i[j]>=97 && i[j]<122)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else
                    {
                        i[j] = 'a';
                    }
                }
            }
            else if(type=="001")
            {
                for(int j=length-1; j>=0; j--)
                {
                    if(i[j]>=48 && i[j]<57)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else
                    {
                        i[j] = '0';
                    }
                }
            }
            else if(type=="010")
            {
                for(int j=length-1; j>=0; j--)
                {
                    if(i[j]>=65 && i[j]<90)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else
                    {
                        i[j] = 'A';
                    }
                }
            }
            else
            {
                for(int j=length-1; j>=0; j--)
                {
                    if(i[j]>=97 && i[j]<122)
                    {
                        i[j] = i[j] + 1;
                        break;
                    }
                    else
                    {
                        i[j] = 'a';
                    }
                }
                break;
            }
            if(count%10000==0)//sending temperary messeges to check whether server is alive or not
            {
                string temp="#";
                int bytes_sent = send(sockfd,temp.c_str(),strlen(temp.c_str()),MSG_NOSIGNAL);
                socklen_t len;
                len=sizeof(struct sockaddr);
                if(getpeername(sockfd,(struct sockaddr *)&dest_addr, &len)==-1)
                {
                    close(sockfd);
                    connNotActive=true;
                    i=finish;
                    break;
                }
            }
        }
        string present=crypt(finish.c_str(),salt.c_str());
        if(i==finish&&present!=hash)//pasword not found
        {
            string t = "@";
            int bytes_sent = send(sockfd,t.c_str(),strlen(t.c_str()),0);
            close(sockfd);
            connNotActive=true;
        }
        else if(i==finish&&present==hash)
        {
            int bytes_sent = send(sockfd,i.c_str(),i.length(),0);
        }
    }
}
