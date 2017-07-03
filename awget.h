;//  header file for Programming Assignment 2
//  header file for Programming Assignment 2
//  Joshua Zarin
//  CS457
//  October 27, 2016
#ifndef PA2_HEAD
#define PA2_HEAD

#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <sys/types.h>  //system call data types
#include <netinet/in.h> //structs for domain addresses
#include <arpa/inet.h>  //inet_addr
#include <netdb.h>      //hostent
#include <unistd.h>     //close sockets
#include <cstring>  	//std::strlen
#include <stdio.h>  	//std::(I/O)
#include <fstream>  	//file io
#include <iostream> 	//std::cout
#include <string>   	//std::string#include <strstream>
#include <sstream>  	//std::stringstream
#include <ctype.h>  	//isdigit
#include <vector>   	//vector
#include <math.h>   	//sqrt
#include <cstdlib>  	//
#include <streambuf>	//stream buffer
#include <functional>	//bit_xor
#include <ctime>
#include <stdlib.h>
#include <signal.h>
#include <iomanip>
#include <pthread.h>
#include <cstdint>
#include <thread> 
using namespace std;//relieves std:: usage
//----------------------------------------------^IMPORTS^------------------------------------------
        /*
            packet (awget/ss.cpp)
        struct for packet to be passed between computers
        @attribute 1: total length of the url to download
        @attribute 2: char array to hold URL
        @attribute 3: size of message containing next ip addresses in chainfile
        @attribute 4: char array to store ip and port (20 chars) with a maximum of 5 items
        @attribute 5: index to tell how many more chain computers there are
        */
struct packet{
    short url_length;
    char URL[255];
    short chainfile_size;
    char chainfile[22*5];
    short index;

};
struct info{
    long count;
    long bitsLeft;
};

	int download(char *URL, int fileCount){
    // string createdDirCommand = "mkdir -p "+to_sting(fileCount);
    // system(createdDirCommand.c_str());
    // string cd=


	string url(URL);
	string command("wget " + url+" -O download"+to_string(fileCount));
    cout<<command<<endl;
	system((const char*)command.c_str());

	return 0;
}




        /*
            returnPacket (ss.cpp)
        struct for the download to be returned through the chained computers
        @attribute 1: the entire downloaded file of 1400 chars
        */
struct returnPacket {
    char file[1400];
};
        /*
            getIP (awget.cpp)
        function to find the IP address of the next chained computer
        @attribute 1: the packet containing IP address and port of the next computer
        @attribute 2: the index of where the IP address ends
        @return: the IP address for the next connection
        */
string getIP(packet p, int index){
	string IP;
	if(index > p.chainfile_size){
		cout << "Error in getIP" << endl;
		return NULL;
	}
	for(int j = 0; j < 22; j++){
		char current = p.chainfile[j+(index*22)];
		if(current == ' '){
			return IP;
		}
		IP += current;
	}
	return IP;
}
        /*
            getPort (awget.cpp)
        function to find the port of the next chained computer
        @attribute 1: the packet containing IP address and port of the next computer
        @attribute 2: the index of where the IP address ends
        @return: the port for the next connection
        */
string getPort(packet p, int index){
	string Port;
	if(index > p.chainfile_size){
		cout << "Error in getIP" << endl;
		return NULL;
	}
	int set = 0;
	for(int j = 0; j < 22; j++){
		char current = p.chainfile[j+(index*22)];
		if(set == 0){	
			if(current == ' '){
				set = 1;
			}
		}else{
			if(current == ' '){
				return Port;
			}else{
				Port += current;
			}
			
		}
	}
	return Port;
}
        /*
            toCharStar (awget/ss.cpp)
        function to convert a packet to a character array
        @attribute 1: the index of where the packet ends
        @attribute 2: the packet being converted
        */
void toCharStar(char * val, packet p){
    memcpy(val, &p, sizeof(p));
}        
		/*
            createPacket (awget/ss.cpp)
        function to create a packet
        @attribute 1: the url of the download
        @attribute 2: the IP address and port of the packet being created
        @attribute 3: how many stepping stones are left
        @attribute 4: location of our current stepping stone information
        @return: new packet that was created
        */
 packet createPacket(char* url,string address[20], int linecount, int index){
    struct packet p;
    strcpy(p.URL,url);        
    p.url_length=strlen(url);
    bzero(p.chainfile,sizeof(p.chainfile));
    for(int i=0;i<linecount;i++){
		for(unsigned int j = 0; (j < address[i].length()) && (j < 22); j++){
			p.chainfile[j+(i*22)]=address[i].at(j);
		}
    }
    //cout << "Start" << endl;
 //    for(unsigned int i = 0; i < sizeof(p.chainfile); i++){
	// 	cout << p.chainfile[i];
	// }
	//cout << "\nEnd" << endl;
    p.chainfile_size=linecount;
    p.index=index;
    return p;
}
void removeFile(string filename)
{
    string deleteCommand = "rm -rf "+filename;


    system(deleteCommand.c_str());
}






#endif  // PA2_HEAD