//  main file 1, awget.cpp, for Programming Project 2
//  Joshua Zarin, Xi Wei, David Sahud
//  CS457
//  October 27, 2016
#include <awget.h>
//----------------------------------------------^IMPORTS^------------------------------------------
//-------------------------------------------------------------------------------------------------
//----------------------------------------------vMAIN_FUNCTIONv------------------------------------
int main(int argc, char* argv[]){
	char* URL;						//stores URL for download
	if(argc > 4 || argc < 2 || argc==3){	//invalid input size
		cerr << "Usage error: program requires three args. Correct usage: 'awget <URL> [-c chainfile]' -OR- 'awget <URL>'." << endl;
		return -1;
	}
	string filename;			//assigned when given by user, otherwise set equal to default chainfile
	URL=argv[1];
	if(argc==2){filename = "chaingang.txt";}							//default chainfile (chaingang)
	else if(argc == 4 && strcmp(argv[2],"-c")==0){filename = argv[3];}	//user given chainfile
	else{			//file specifications were not correct
		cerr << "Usage error. Correct usage: 'awget <URL> [-c chainfile]' -OR- 'awget <URL>'." << endl;
		return -1;
	}
	ifstream chainfile;			//create file to store IP addresses for chain
	chainfile.open(filename);	//open chain file for reading
	if(!chainfile.is_open()){
		cerr << "Error: could not open file " << filename << "." << endl;
		return -1;
	}
	cout << "Request: " << URL << '\n' << "chainlist is" << endl;
//----------------------------------------------^IO_ERROR_CHECKS^----------------------------------
	string fLine;
	getline(chainfile,fLine);						//getting number of links from chain file
	srand(time(0));									//current time for random seed
	int random_variable = rand();
	int lineCount=stoi(fLine);						//generate random number
	random_variable = random_variable % lineCount;	//convert to random number to select next IP
	string templine;
//----------------------------------------------vREAD/STORE_ADDRESS/PORTv--------------------------
	string address[lineCount];
	for(int i=0;i<lineCount;i++){
		getline(chainfile,templine);
		address[i]=templine;
		cout << "<" << address[i] << ">" << endl;
	}
	string delimiter = " ";
	string destinationIp = address[random_variable].substr(0, address[random_variable].find(delimiter));
	int  destinationPort =stoi(address[random_variable].substr(address[random_variable].find(delimiter)+1,address[random_variable].length()));
	cout << "next SS is <" << destinationIp << ", " << destinationPort << ">" << endl;
	// cout << "Destination IP: " <<destinationIp << endl;
	// cout << "Destination Port: " << destinationPort << endl;
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
		cout << "Connected to SS!" << endl;
	}
//----------------------------------------------vCREATE_PACKETv------------------------------------
	packet p=createPacket(URL,address,lineCount,random_variable);
	// cout << "size of p:" << sizeof(p) << endl;
	// cout << "URL len:" << p.url_length << endl;
	// cout << "URL: " << p.URL << endl;
	// cout << "Chainfile size: " << p.chainfile_size << endl;
	// cout << "sizeeeeeeeeee: "<< sizeof(p.chainfile) << endl;
	// for(int i = 0; i < p.chainfile_size; i++){
	// 	cout << "addr: " << getIP(p,i) << " port: " << getPort(p,i) << endl;//p.chainfile[i] << endl;
	// }
	char bitpack [sizeof(p)];
	bzero(bitpack, sizeof(bitpack));
	toCharStar(bitpack,p);
	//cout<< bitpack<<endl;
//----------------------------------------------vSEND/RECEIVE_PACKAGEv-----------------------------
	if(send(socketConn, bitpack, sizeof(bitpack),0) < 0){
		cerr << "Error sending informafion." << endl;
		return -1;
	}
	cout << "waiting for file..." << endl;
	// cout << sizeof(bitpack)<<endl;
	// cout << "Sent" << endl;
	// cout << "waiting to receive"<<endl;

string saveFileName;
string url(p.URL);
	int position = url.find_last_of("/");
  if(position==-1){
    saveFileName= "index.html";
  }
  else{
    saveFileName = url.substr(position+1);
  }
  string temp=" ";
  if(saveFileName.compare(" ")==0){
     saveFileName= "index.html";
  }

// string saveFileName="download"+to_string(fileCount);
// 	fileCount++;
	
//	char inputBuffer[14000];
	//bool dataComplete = false;
	//std::vector<unsigned char> fullOutput;
	int bytes;

	ofstream dataFile;
	dataFile.open(saveFileName, ios::out | ios::binary | ios::ate);


	//int totalBytes = 0;
		
	info pacInfo;
            char infobuffer[sizeof(pacInfo)];
            recv(socketConn, (void *)&infobuffer, sizeof(infobuffer), MSG_WAITALL);
            memcpy(&pacInfo,infobuffer,sizeof(infobuffer));
            // cout<<"pacinfo count: "<<pacInfo.count<<endl;
            // cout<<"pacinfo remain:"<<pacInfo.bitsLeft<<endl;
            cout << ".." << endl;
			for(int i=0;i<pacInfo.count;i++){

            		if(i+1==pacInfo.count){
            			char inputBuffer[pacInfo.bitsLeft];
            			bytes = recv(socketConn, (void *)&inputBuffer, sizeof(inputBuffer), MSG_WAITALL);
            			dataFile.write(inputBuffer, bytes);
            		}else{
            			char inputBuffer[14000];
            			bytes = recv(socketConn, (void *)&inputBuffer, sizeof(inputBuffer), MSG_WAITALL);
            			dataFile.write(inputBuffer, bytes);

            		}

            		



            }

	 cout << "Received file <" <<  saveFileName << ">" << '\n' << "Goodbye!" << endl;
	// while(!dataComplete)
	// 	 {
	// 		bytes = recv(socketConn, (void *)&inputBuffer, sizeof(inputBuffer), 0);
	// 		cout<<"got one size:"<<bytes<<endl;
	// 		if(bytes < 0)
	// 		{
	// 			cerr<<"Unable to receive file successfully "<<endl;
	// 			exit(1);
	// 		}

	// 		if(bytes>0)
	// 		{   
	// 			dataFile.write(inputBuffer, bytes);

	// 		}
	// 		else
	// 		{	
	// 			//dataFile.write(inputBuffer, bytes);
	// 			dataComplete = true;
	// 			cout<<"finish"<<endl;
	// 		}

	// 		totalBytes += bytes;
	// 	 }
	dataFile.close();
	close(socketConn);






	chainfile.close();				//close file for IP addresses
	return 0;
}
