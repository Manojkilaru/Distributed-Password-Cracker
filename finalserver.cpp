#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <queue>
#include <map>
#include <vector>

using namespace std;
class User
{
public:
    string hash;
    int length;
    string type;
    int fd;
    int workers[5];
    User(string a,int b, string c,int d)
    {
        hash=a;
        length=b;
        type=c;
        fd=d;
        for(int i=0; i<5; i++)
        {
            workers[i]=-1;
        }

    }
};
class Worker
{
public:
    int user;
    int fd;
    bool free;
    Worker(int b,int d)
    {
        free=1;
        user=b;
        fd=d;
    }
};

using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char* argv[])
{
    vector<User> user_list;//here we will strore the users
    queue<User> priority;// here we will store the users in the queue
    vector<Worker> worker_list;// here we will store the workers in the list
    fd_set master;    // master file descriptor list
    fd_set read_fds,write_fds;  // temp file descriptor list for select()
    int fdmax;
    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_in remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;
    int cnt=0;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    char *PORT=argv[1];
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }//used for connecting to clients

    for(p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
        {
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    if (listen(listener, 10) == -1)
    {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    bool otherUsers=0,flag2=0;
    // main loop
    map<int,int> worker_client;//is used to find who is worker or who is user
    for(;;)
    {
        //cout<<"lanja"<<endl;
        //cout<<worker_list.size()<<"intial"<<endl;
        if (!priority.empty()&&otherUsers==0)//if priority queue is empty or other users qre left
        {
            int numfreeusers=0;
            for (int p = 0; p < worker_list.size(); ++p)
            {

                if (worker_list[p].free)
                {
                    numfreeusers++;//finding number of users
                }
            }
            if(numfreeusers!=0)
            {
                vector<string> work1(numfreeusers);//storing the work of first part for all free workers
                vector<string> work2(numfreeusers);//storing the work of second part for all free workers
                string b = (priority.front()).type;//type of password
                int len = (priority.front()).length;

                if(b == "111")
                {
                    string last(len,'z');
                    char start = '0';
                    int j=0;
                    for(j=0; j!=numfreeusers-1; j++)
                    {
                        string tt(len,'0');
                        tt[0]=start;
                        work1[j]=tt;
                        work2[j]=tt;
                        work2[j][0]=start+62/numfreeusers;
                        start=start+62/numfreeusers;
                    }
                    string tt(len,'0');
                    tt[0]=start;
                    work1[j]=tt;
                    work2[j]=last;
                }//dividing first char into number of free workers and left rest all the same
                else if(b == "110")
                {
                    string last(len,'z');
                    char start = 'A';
                    int j=0;
                    for(j=0; j!=numfreeusers-1; j++)
                    {
                        string tt(len,'A');
                        tt[0]=start;
                        work1[j]=tt;
                        work2[j]=tt;
                        work2[j][0]=start+52/numfreeusers;
                        start=start+52/numfreeusers;
                    }
                    string tt(len,'A');
                    tt[0]=start;
                    work1[j]=tt;
                    work2[j]=last;
                }//similar as above
                else if(b == "101")
                {
                    string last(len,'z');
                    char start = '0';
                    int j=0;
                    for(j=0; j!=numfreeusers-1; j++)
                    {
                        string tt(len,'0');
                        tt[0]=start;
                        work1[j]=tt;
                        work2[j]=tt;
                        work2[j][0]=start+36/numfreeusers;
                        start=start+36/numfreeusers;
                    }
                    string tt(len,'0');
                    tt[0]=start;
                    work1[j]=tt;
                    work2[j]=last;
                }
                else if(b == "100")
                {
                    string last(len,'z');
                    char start = 'a';
                    int j=0;
                    for( j=0; j!=numfreeusers-1; j++)
                    {
                        string tt(len,'a');
                        tt[0]=start;
                        work1[j]=tt;
                        work2[j]=tt;
                        work2[j][0]=start+26/numfreeusers;
                        start=start+26/numfreeusers;
                    }
                    string tt(len,'a');
                    tt[0]=start;
                    work1[j]=tt;
                    work2[j]=last;
                }
                else if(b == "001")
                {
                    string last(len,'9');
                    char start = '0';
                    int j=0;
                    for(; j!=numfreeusers-1; j++)
                    {
                        string tt(len,'0');
                        tt[0]=start;
                        work1[j]=tt;
                        work2[j]=tt;
                        work2[j][0]=start+10/numfreeusers;
                        start=start+10/numfreeusers;
                    }
                    string tt(len,'0');
                    tt[0]=start;
                    work1[j]=tt;
                    work2[j]=last;
                }
                else if(b == "010")
                {
                    string last(len,'Z');
                    char start = 'A';
                    int j=0;
                    for(; j!=numfreeusers-1; j++)
                    {
                        string tt(len,'A');
                        tt[0]=start;
                        work1[j]=tt;
                        work2[j]=tt;
                        work2[j][0]=start+26/numfreeusers;
                        start=start+26/numfreeusers;
                    }
                    string tt(len,'A');
                    tt[0]=start;
                    work1[j]=tt;
                    work2[j]=last;
                }
                else if(b == "011")
                {
                    string last(len,'Z');
                    char start = '0';
                    int j=0;
                    for(j=0; j!=numfreeusers-1; j++)
                    {
                        string tt(len,'0');
                        tt[0]=start;
                        work1[j]=tt;
                        work2[j]=tt;
                        work2[j][0]=start+36/numfreeusers;
                        start=start+36/numfreeusers;
                    }
                    string tt(len,'0');
                    tt[0]=start;
                    work1[j]=tt;
                    work2[j]=last;
                }

                int userid;// finding the exact user in the list so that we can update his information in structure
                for(int k=0; k!=user_list.size(); k++)
                {
                    if(user_list[k].fd==(priority.front()).fd)
                    {
                        userid=k;// got that user
                        break;
                    }
                }
                int l=0;
                for (int p = 0; p < worker_list.size(); ++p)
                {
                    if (worker_list[p].free)
                    {
                        worker_list[p].free=false;// making the worker not free
                        worker_list[p].user=(priority.front()).fd; // storing id of user in worker
                        (user_list[userid]).workers[l]=(worker_list[p]).fd;
                        string tempp = work1[l]+" "+work2[l]+" "+b+" "+(user_list[userid]).hash;
                        send(worker_list[p].fd,tempp.c_str(),strlen(tempp.c_str()),0);// sending this data to worker
                        l++;
                    }
                }
                priority.pop();
            }
        }
        read_fds = master; // storing the master information in readfds
        write_fds = master; // storing the master information in readfds
        timeval tv;
        tv.tv_sec=10;
        tv.tv_usec=0;// using this time as timeout for select
        if (select(fdmax+1, &read_fds,&write_fds, NULL, &tv) == -1)
        {
            perror("select");
            exit(4);
        }
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))   // we got one!!
            {

                if (i == listener)
                {
                    // handle new connections as this is id of listener
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);// acepting the connection
                    if (newfd == -1)
                    {
                        perror("accept");// eror in connecting
                    }
                    else
                    {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax)      // increasing the max
                        {
                            fdmax = newfd;
                        }
                        worker_client[newfd]=0;
                    }
                }
                else
                {
                    bzero(buf,sizeof buf);
                    nbytes=recv(i, buf, sizeof buf, 0);
                    buf[nbytes] = '\0';
                    if (nbytes <= 0)//number of bytes reveived is lessthan 0
                    {
                        if (nbytes == 0)// number of bytes recieved is 0
                        {
                            // connection closed
                            for(int kk=0; kk!=worker_list.size(); kk++)
                            {
                                if(worker_list[kk].fd == i)
                                {
                                    worker_list.erase(worker_list.begin()+kk);
                                }// connection was closed so removing from the worker list
                            }// finding the exact worker from whom the error came
                        }
                        else
                        {
                            perror("recv");//there some other error rather than close
                        }
                        close(i); //closing the socket
                        FD_CLR(i, &master); // remove from master set
                    }
                    else
                    {
                        if(worker_client[i]==0)//we dont who is he
                        {
                            if(buf[0]=='1')
                            {
                                worker_client[i]=1;// the id is user
                            }
                            else if(buf[0]=='0')// the id is worker
                            {
                                worker_client[i]=-1;
                                Worker temp(-1,i);
                                worker_list.push_back(temp);
                                if(otherUsers==1)
                                {
                                    cnt--;
                                }
                                if(cnt==0)
                                {
                                    otherUsers=0;
                                }// other users added
                            }
                        }
                        else if(worker_client[i]==1)
                        {
                            string hash,len,type;
                            int j=0;
                            for (; j < nbytes&&buf[j]!=' '; ++j)
                            {
                                hash+=buf[j];
                            }
                            j++;
                            for (; j < nbytes&&buf[j]!=' '; ++j)
                            {
                                len+=buf[j];
                            }
                            j++;
                            for (; j < nbytes&&buf[j]!=' '; ++j)
                            {
                                type+=buf[j];
                            }// gathering the information from user
                            User temp(hash,stoi(len),type,i);
                            user_list.push_back(temp);// added the information to the list
                            priority.push(temp);//this is added at the back
                        }
                        else
                        {
                            int userid;//user's index no in user_list
                            if(buf[0]!='#')
                            {
                                for(int k=0; k<worker_list.size(); k++)//freeing the worker
                                {
                                    if(worker_list[k].fd==i)
                                    {
                                        worker_list[k].free=1;
                                        for(int q=0; q<user_list.size(); q++) //obtaining userid
                                        {
                                            if(user_list[q].fd==worker_list[k].user)
                                            {
                                                userid=q;// found the user whose answer was returned
                                            }
                                        }
                                    }
                                }
                            }
                            if(buf[0]=='@')//no answer found
                            {
                                for(int q=0; q<5; q++)//removing the worker from the user's worker array
                                {
                                    if(user_list[userid].workers[q]==i)
                                    {
                                        user_list[userid].workers[q]=-1;
                                    }
                                }
                                for(int l=0; l<worker_list.size(); l++)//removing him from worker list
                                {
                                    if(worker_list[l].fd==i)
                                    {
                                        worker_list.erase(worker_list.begin()+l);
                                        break;
                                    }
                                }
                                FD_CLR(i,&master);//remove from master
                                close(i);
                                worker_client.erase(i);
                                bool noworker=0;
                                for(int q=0; q<5; q++)
                                {
                                    if(user_list[userid].workers[q]!=-1)
                                    {
                                        noworker=1;
                                        break;
                                    }
                                }
                                if(noworker==0)// if he is the last worker
                                {
                                    string temp="sorry password not found";
                                    send(user_list[userid].fd,temp.c_str(),strlen(temp.c_str()),0);
                                    worker_client.erase(user_list[userid].fd);
                                    user_list.erase(user_list.begin()+userid);
                                    FD_CLR(user_list[userid].fd,&master);
                                    close(user_list[userid].fd);
                                }
                            }
                            else if(buf[0]!='#')
                            {
                                send(user_list[userid].fd,buf,strlen(buf),0);
                                FD_CLR(user_list[userid].fd,&master);//removes user from master
                                close(user_list[userid].fd);
                                for(int q=0; q<5; q++)//removes all coworkers
                                {
                                    if(user_list[userid].workers[q]!=-1)
                                    {
                                        for(int l=0; l<worker_list.size(); l++)
                                        {
                                            if(worker_list[l].fd==user_list[userid].workers[q]&&worker_list[l].fd!=i)
                                            {
                                                worker_list.erase(worker_list.begin()+l);
                                                break;
                                            }
                                        }
                                        if(user_list[userid].workers[q]!=i)
                                        {
                                            worker_client.erase(user_list[userid].workers[q]);
                                            // cout<<worker_list.size()<<endl;
                                            FD_CLR(user_list[userid].workers[q],&master);
                                            close(user_list[userid].workers[q]);
                                            cout<<"others closed"<<endl;
                                            otherUsers=1;
                                            cnt++;
                                        }
                                    }
                                }
                                worker_client.erase(user_list[userid].fd);//removes user reg
                                user_list.erase(user_list.begin()+userid);//removes user
                                cout<<worker_list.size()<<endl;
                            }
                            continue;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
