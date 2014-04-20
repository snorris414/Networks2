using namespace std;
#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;
#include <stdio.h>
#include "unp.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <sys/select.h>
#include <sys/time.h>
#include <istream>
#include <ostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <pthread.h>

int computeCheckSum(vector<unsigned char> bytes);
int sequenceNumber = 0;
int expectedNumber = 15;
unsigned char charArray[501];
string packetToString(struct Packet packet);
string convertVectorToString(vector<unsigned char> vec);
string intToString(int number);
int stringToInt(string str);
struct Header
  {
	  int seqNum;
	  int checksum;
	  int ACK_NAK;
  };
void receiveAndResend(int sd, sockaddr_in server, Header header, vector<unsigned char> byteBuffer, double prob);
void gremlin(Packet *packet, double prob);
vector<unsigned char> charArrayToVector(unsigned char charArr[], int arrSize, vector<unsigned char> byteBuffer, FILE *fp);

int computeCheckSum(vector<unsigned char> bytes){
	int checkSum = 0;
//	int arrLength = sizeof(bytes)/sizeof(bytes[0]);
	for (int i=0; i < (int) bytes.size(); i++){
		checkSum += bytes[i];
	}
	return checkSum;
}

int flipInt(int integer) {
	if (integer == 1) {
		integer = 0;
	}
	else if (integer == 0) {
		integer = 1;
	}
	else {
		 printf("Tried to flip non-binary integer.");
		 sleep(2000);
		 exit(0);
	}
	return integer;
}

  Header dummy_header = {0,0,0};
  int HEADER_SIZE = 11;
  int DATA_SIZE = 512 - HEADER_SIZE;
  struct Packet
  {
      	  Header header;
      	  vector<unsigned char> data;
  };
  const int WINDOW_SIZE = 16;
  Packet packetArr[WINDOW_SIZE];

void buildArr(Packet *packetArr, FILE *fp);

int main(int argc, char **argv) {
  int sd;
  struct sockaddr_in server;
  struct hostent *hp;
  int filesize;
  int numPackets;
  int sequenceNum;
  srand(time(NULL));

  FILE *fp;

    sequenceNum = 0;
  	vector<unsigned char> byteBuffer;




  sd = socket(AF_INET, SOCK_DGRAM, 0);

  server.sin_family = AF_INET;
  server.sin_port = htons(SERV_PORT);
  hp = gethostbyname(argv[1]);
  bcopy(hp->h_addr, &(server.sin_addr), hp->h_length);
  ifstream ifs;
  bool quit = false;
  string filename;
  while (!quit) {
	  cout << "ftp> ";
//	  string dummy;
//	  cin >> dummy;
//	  getline(cin, dummy);
	  vector<string> stringVec;
	  string line;
	  getline(cin, line);
	  boost::split(stringVec, line, boost::is_any_of("\t "));
	  if (stringVec.size() == 1) {
		  string temp = stringVec.at(0);
		  if (temp.compare("quit") != 0) {
			  cout << "You did not enter a supported command. Please enter one of the following:" << endl;
			  cout << "Commands: quit, PUT <filename>" << endl;
		  }
		  else {
			  quit = true;
		  }
	  }
	  else if (stringVec.size() == 2) {
		  if (stringVec.at(0).compare("PUT") == 0) {
			  filename = stringVec.at(1);
			  cout << stringVec.at(0) << " " << stringVec.at(1) << endl;
			  ifs.open(filename.c_str(), std::ifstream::binary);
//			  fp = fopen(filename.c_str(), "rb");
			    if (fp == NULL) {
			    	cout << "File not found." << endl;
			    }
			    else {
			    	quit = true;
			    }
		  }
		  else {
			  cout << "You did not enter a supported command. Please enter one of the following:" << endl;
			  cout << "Commands: quit, PUT <filename>" << endl;
		  }
	  }
	  else {
		  cout << "You did not enter a supported command. Please enter one of the following:" << endl;
		  cout << "Commands: quit, PUT <filename>" << endl;
		  for (int i = 0; i < stringVec.size(); i++) {
			  cout << stringVec.at(i) << " " << endl;
		  }
	  }
  }

  while (!ifs.eof()){
	  buildArr(packetArr, fp);
	  send();
  }

//  double prob = atof(argv[2]); //need 3 probablilites
//  				  	  Packet *packetPtr = &packet;
//  				  	  gremlin(packetPtr, prob);
//
//	  	  string packetString = packetToString(packet);
//	  	  cout << packetString << endl;
//
//	  if (packet.header.ACK_NAK != -1) {
//		  sendto(sd, packetString.c_str(), strlen(packetString.c_str()), 0, (struct sockaddr *) &server, sizeof(server));
//	  }
//	  receiveAndResend(sd, server, header, byteBuffer, prob);

//  }


  char arr[1];
  arr[0] = NULL;
  sendto(sd, arr, 1, 0, (struct sockaddr *) &server, sizeof(server));
  cout << "ftp> PUT successfully completed" << endl;
  close(sd);
}


string packetToString(Packet packet) {
	string str;
	str = intToString(packet.header.seqNum) + " ";
	if (packet.header.checksum < 10000) {
		str += intToString(packet.header.checksum) + "  ";
	}
	else {
		str += intToString(packet.header.checksum) + " ";
	}
	str += intToString(packet.header.ACK_NAK) + " ";
	str += convertVectorToString(packet.data);
	string data = convertVectorToString(packet.data);
	return str;
}

string convertVectorToString(vector<unsigned char> vec) {
	string str = "";
	for (int i = 0; i < (int) vec.size(); i++) {
		str += vec.at(i);
	}
	return str;
}

vector<unsigned char> charArrayToVector(unsigned char charArr[], int arrSize) {
	vector<unsigned char> vec;
	for (int i=0; i < arrSize; i++){
		vec.push_back(charArr[i]);
	}
	return vec;
}

string intToString(int number)
{
   stringstream ss;
   ss << number;
   return ss.str();
}

int stringToInt(string str) {
	stringstream ss(str);
	int num;
	ss >> num;
	return num;
}

string charToString(char c) {
	string str;
	stringstream ss;
	ss << c;
	ss >> str;
	return str;
}

/*
 *  Receives ACK/NAK from server. Resend the packet if a NAK is received or
 *  the wait times out. Time-out is 20 milliseconds.
 */
void receiveAndResend(int sd, sockaddr_in server, Header header, vector<unsigned char> byteBuffer, double prob) {
	// Handle ACK/NAK interaction
	  int seq_num = NULL;


	  // wait for ACK/NAK for 20 milliseconds
	  int n;
	  char buf[512];
	  fd_set fd;
	  FD_ZERO(&fd);
	  FD_SET(sd,&fd);
	  timeval timer;
	  timer.tv_sec = 0;
	  timer.tv_usec = 20000;
	  int selectval = select(sd + 1, &fd, NULL, NULL, &timer);
	  if (selectval != -1 && selectval != 0) {
		  n = recv (sd, buf, sizeof(buf), 0);
		  buf[n] = '\0';
		  string ackString = charToString(buf[2]);
		  seq_num = stringToInt(ackString);
//		  printf("Received: %s\n", buf);
	  }
	  if (ACK_NAK == NULL || ACK_NAK == 0) {
		  Packet packet = {header, byteBuffer};
		  Packet *packetPtr = &packet;
		  gremlin(packetPtr, prob);
		  string packetString = packetToString(packet);
		  cout << packetString << endl;
		  if (packet.header.ACK_NAK != -1) {
			  sendto(sd, packetString.c_str(), strlen(packetString.c_str()), 0, (struct sockaddr *) &server, sizeof(server));
		  }
		  else {
			  cout << "A packet was lost. Sequence Num: " << intToString(packet.header.seqNum) << endl;
		  }
		  receiveAndResend(sd, server, header, byteBuffer, prob);
	  }
}

void buildArr(Packet *packetArr, FILE *fp) {
	vector<unsigned char> byteBuffer;
	if (sequenceNumber == expectedNumber){
		for (int i = 0; i < WINDOW_SIZE; i++) {
				  fread(charArray, 1, 501, fp);
				  int sizeOfArr = sizeof(charArray)/sizeof(*charArray);
				  byteBuffer = charArrayToVector(charArray, sizeOfArr);

				  Header header = {sequenceNumber,0,0};
				  header.checksum = computeCheckSum(byteBuffer);
				  header.ACK_NAK = 0;
				  sequenceNumber = (sequenceNumber+1)%32;

				  Packet packet = {header, byteBuffer};
				  packetArr[i] = packet;

				  for (int i = 0; i < 501; i++) {
					  charArray[i] = NULL;
				  }
		}
	}
	else{
		int j = 0;
		for (int i = sequenceNumber%16; i < WINDOW_SIZE; i++){
			packetArr[j] = packetArr[i];
			j++;
		}

		for (int i = j; i < WINDOW_SIZE; i++) {
			  fread(charArray, 1, 501, fp);
			  int sizeOfArr = sizeof(charArray)/sizeof(*charArray);
			  byteBuffer = charArrayToVector(charArray, sizeOfArr);

			  Header header = {sequenceNumber,0,0};
			  header.checksum = computeCheckSum(byteBuffer);
			  header.ACK_NAK = 0;
			  sequenceNumber = (sequenceNumber+1)%32;

			  Packet packet = {header, byteBuffer};
			  packetArr[i] = packet;

			  for (int i = 0; i < 501; i++) {
				  charArray[i] = NULL;
			  }
		}
	}

}

void gremlin(Packet *packet, double prob) {

	float randomNum = (float) rand()/ (float) RAND_MAX;
	float damageChance = prob;
	if (damageChance > randomNum) {
		int randNum2 = rand()%2;
		if (randNum2 == 0) { // Drop packet
			packet->header.ACK_NAK = -1;
		}
		else {
				float dataSize = (float) 1 / (float) 501;
				for (int i = 0; i < 501; i++) {
					float randNum3 = (float) rand()/ (float) RAND_MAX;
					if (randNum3 <= dataSize) {
						packet->data[i] += 5;
				}
			}
		}
	}
}
