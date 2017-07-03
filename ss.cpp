//  main file 2, ss.cpp, for Programming Project 2
//  Joshua Zarin
//  CS457
//  October 27, 2016
#include <awget.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <iostream>
#include <iomanip>


using namespace std;
int filecCount=0;



#define DefaultPort 3428
int port;
int yes=1;
struct hostent *he;
char hostname[128];
struct in_addr **addr_list;

    // printf("My host name: %s\n",hostname);
int sockfd;  
struct addrinfo hints, *servinfo, *p;
int rv;

    //variables for server
int server_sockfd;
socklen_t addr_size;
struct sockaddr_in server_addr;
struct sockaddr_storage their_addr;

    // variables for client
int client_sockfd;
struct sockaddr_in client_addr;
struct hostent *hostinfo;

//signal handler method
void intHandler(int dummy) {
  close(server_sockfd);
  close(client_sockfd);
//    printf("signal working\n");
  exit(0);
}

int sendNextStone(packet recPack){

  string address[20];
  int index=0;
  cout << "Request: " << recPack.URL << endl;
  for(int i=0;i<recPack.chainfile_size;i++){
    if(i!=recPack.index){
      address[index]=getIP(recPack,i)+" "+getPort(recPack,i);
      index++;
    }   
  }


  srand(time(0));                                    //current time for random seed
  int random_variable = rand();
  random_variable = random_variable % index;
  packet p=createPacket(recPack.URL,address,index,random_variable);
  cout<<"chainlist is"<<endl;
  for(int i = 0; i <p.chainfile_size; i++){
    cout <<"<" <<getIP(p,i) << ", " << getPort(p,i) <<">"<< endl;
  }

//------
  string destinationIp = getIP(p,random_variable);
  int  destinationPort =stoi(getPort(p,random_variable));

  cout<<"next SS is <"<<destinationIp<<"," <<destinationPort<<">"<<endl;
//----------------------------------------------vCREATE_SOCKETv------------------------------------
  struct sockaddr_in server;
  int socketConn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(socketConn < 0){
    cerr << "Error creating socket." << endl;
    return -1;
  }
  server.sin_family = AF_INET;
  server.sin_port = htons(destinationPort);
  server.sin_addr.s_addr = inet_addr(destinationIp.c_str());
  //cout << "Connecting to server... ";
  int connectCheck = connect(socketConn,(struct sockaddr *)&server, sizeof(server));
  if(connectCheck < 0){
    cerr << "Error connecting to server." << endl;
    return -1;
  }else{
    // cout << "waiting for file..." >> '\n' << ".." << endl;
  }

  char bitpack [sizeof(p)];
  bzero(bitpack, sizeof(bitpack));
  toCharStar(bitpack,p);
  //cout<< bitpack<<endl;
//----------------------------------------------vSEND/RECEIVE_PACKAGEv-----------------------------
  if(send(socketConn, bitpack, sizeof(bitpack),0) < 0){
    cerr << "Error sending informafion." << endl;
    return -1;
  }

  //cout << sizeof(bitpack)<<endl;
  //cout << "Sent" << endl;

  //Wating for request to return.
  return socketConn;
}
int finalStone(char URL[255], int client_sockfd){
   //download the file
  cout<<"Request:"<< URL << endl;
  cout<<"chainlist is empty"<<endl;
  
  string filename;
  filename="download"+to_string(filecCount);
  filecCount++;
  download(URL, filecCount-1);
  string url(URL);
  

  ifstream fileStream;
  fileStream.open(filename,ios::binary);
  if(!fileStream.is_open()){
    cerr << "Error: could not open file " << filename << "." << endl;
    return -1;
  }
  fileStream.seekg (0, ios::end);
  int length = fileStream.tellg();
  fileStream.seekg(0, ios::beg);

  char buffer[14000];
  //cout<<"buffer length:"<<length<<endl;
  if(fileStream.is_open())
  {
   cout<<"File received .."<<endl;
   cout<<"Relaying file... "<<endl;

   int count =0;
   if(length<=14000){
      count=1;
   }
   else{
    count=length/14000+1;
   }
  //cout<<"count: "<<count<<endl;
  info pacInfo;
  pacInfo.count=count;
  pacInfo.bitsLeft=length%14000;
  char infobuffer[sizeof(pacInfo)];
  memcpy(infobuffer, &pacInfo, sizeof(pacInfo));     
  send(client_sockfd, infobuffer, sizeof(infobuffer), 0);
  for(int i=0;i<count;i++){
  bzero(buffer, sizeof(buffer));
    if(i!=count-1){
      fileStream.read(buffer,14000);
      send(client_sockfd, &buffer, 14000, 0);
      //cout<<"size of sent bytes:"<<bytes<<endl;
    }
    else
    {
      fileStream.read(buffer, length%14000);
     send(client_sockfd, &buffer, length%14000, 0);
      //cout<<"size of sent bytes:"<<bytes<<endl;
    }
  }
}
  fileStream.close();
  removeFile(filename);
  cout<<"Goodbye!"<<endl;
  close(client_sockfd);
  return 0;

}

int forwardDataBack(int socketConn,int client_sockfd){
  //Check pacInfo which contains the total packets expected and the remainder in the last packet.
  info pacInfo;
  char infobuffer[sizeof(pacInfo)];
  cout << "waiting for file..." << '\n' << ".." << endl;
  recv(socketConn, (void *)&infobuffer, sizeof(infobuffer), MSG_WAITALL);
  cout<<"Relaying file ..."<<endl;
  memcpy(&pacInfo,infobuffer,sizeof(infobuffer));

  send(client_sockfd, infobuffer, sizeof(infobuffer), 0);

  //Loop through the number of packets expected.
  for(int i=0;i<pacInfo.count;i++){
    //Last packet may not be full
    if(i+1==pacInfo.count){
      char inputBuffer[pacInfo.bitsLeft];
      recv(socketConn, (void *)&inputBuffer, sizeof(inputBuffer), MSG_WAITALL);
      send(client_sockfd,(void *)&inputBuffer, sizeof(inputBuffer), 0);
    }else{
      char inputBuffer[14000];
      recv(socketConn, (void *)&inputBuffer, sizeof(inputBuffer), MSG_WAITALL);
      send(client_sockfd,(void *)&inputBuffer, sizeof(inputBuffer), 0);
    }
  }   
  cout<<"Goodbye!"<<endl;
  close(socketConn);



  return 0; 
}

int runSS(int client_sockfd){
  // int client_sockfd;
  //  client_sockfd = (int)client_sockfd_test;
  if(client_sockfd<0){
  printf("error connecting");
  exit(0);
  }



packet recPack;
//cout<<"size of recPack:"<<sizeof(recPack)<<endl;
    //bzero(recPack.URL, sizeof(recPack.URL));    
char recBitPack[sizeof(recPack)];
    //bzero(recBitPack, sizeof(recBitPack));
int value = recv(client_sockfd,recBitPack,sizeof(recBitPack),MSG_WAITALL);
if (value < 0){
  cout << "Error reading in value:" << value << endl;
    //  cout << "last error: " << xn_getlasterror() << endl;
  //return -1;
}

    //bzero(recPack.URL, sizeof(recPack.URL));
//cout << "Packet recieved" << endl;
//cout << sizeof(recBitPack) <<endl;
    //cout << recBitPack <<endl;
memcpy(&recPack,recBitPack,sizeof(recBitPack));
//cout << recPack.url_length<< endl;

  /*for(int i = 0; i < 110; i++){
    cout << recPack.chainfile[i];
  }*/


    // take itself out from the chainfile
   if(recPack.chainfile_size==1){
     //add function here
    finalStone(recPack.URL, client_sockfd);
  }
  else
  {
  int socketConn = sendNextStone(recPack);
  forwardDataBack(socketConn,client_sockfd);
  } 
  return 0;
}




int main(int argc, char **argv) 
{
   signal(SIGINT, intHandler);
   int port=atoi(argv[2]);
   if(port > 66535 || port < 0){
    cerr << "Error: bad port number" << endl;
    return -1;
   }

   gethostname(hostname, sizeof hostname);
      // main code for server
   server_sockfd=socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);

   if(server_sockfd<0){
    printf("error");
  }

  if (setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
    perror("Setsockopt");
    exit(1);
  }
  //printf("Weclome to Chat!\n");
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(port);
  he=gethostbyname(hostname);
  addr_list = (struct in_addr **)he->h_addr_list;
  server_addr.sin_addr = *((struct in_addr **)he->h_addr_list)[0];
  string ip=inet_ntoa(*addr_list[0]);

  printf("Waiting for a connection on %s port %d\n", inet_ntoa(*addr_list[0]), port);
          //binding
  int b=bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
  //cout<<"bind success:"<<b<<endl;
  if(b<0){
    cerr<<"error binding"<<endl;
    exit(1);
  }
  listen(server_sockfd,10000);
  printf("listening \n");

  addr_size = sizeof their_addr;
  
  //vector<thread>= thrls;
  while(true){
    client_sockfd = accept(server_sockfd, (struct sockaddr *)&their_addr, &addr_size);
    //client_sockfd_test = accept(server_sockfd, (struct sockaddr *)&their_addr, &addr_size);
    //void *arg = malloc(sizeof(*arg));
    //memcpy(infobuffer, &pacInfo, sizeof(pacInfo));  
    //*arg = client_sockfd;
    //pthread_t thread;

    //pthread_create(&thread, NULL, runSS,(void*) client_sockfd);
  //  runSS(client_sockfd);
      thread runss(runSS, client_sockfd);
      //thread runss1(runSS, client_sockfd);
      runss.join();
      //runss1.join();

  }
  return 0;
}






