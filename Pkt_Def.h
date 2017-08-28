#include <iostream>
#include <iomanip>
#include <stdio.h>

#ifndef PKTDEF_H
#define PKTDEF_H

#define FORWARD 1
#define BACKWARD 2
#define RIGHT 3
#define LEFT 4
#define UP 5
#define DOWN 6
#define OPEN 7
#define CLOSE 8
#define HEADERSIZE 6

enum CmdType {DRIVE = 1, STATUS, SLEEP, ARM, CLAW, ACK, NACK};

struct Header
{
	unsigned int pktCount;
	unsigned char Drive : 1;
	unsigned char Status : 1;
	unsigned char Sleep : 1;
	unsigned char Arm : 1;
	unsigned char Claw : 1;
	unsigned char Ack : 1;
	unsigned char Padding : 2;
	unsigned char length;
};

struct MotorBody
{
	unsigned char direction;
	unsigned char duration;
};

struct TelBody
{
	unsigned short int sonar;
	unsigned short int armPos;
	unsigned char Drive : 1;
	unsigned char ArmUp : 1;
	unsigned char ArmDown : 1;
	unsigned char ClawOpen : 1;
	unsigned char ClawClosed : 1;
	unsigned char Padding : 3;
};

class PktDef
{
	private:
		struct CmdPacket
		{
			Header header;
			char* data;
			char CRC;
		} comPkt;

		char* RawBuffer;

	public:
		PktDef();
		PktDef(char*);
		void SetCmd(CmdType);
		void SetBodyData(char*, int);
		void SetPktCount(int);
		CmdType GetCmd();
		bool GetAck();
		bool GetStatus();
		int GetLength();
		char* GetBodyData();
		int GetPktCount();
		bool CheckCRC(char*, int);
		void CalcCRC();
		char* GenPacket();
};

#endif
