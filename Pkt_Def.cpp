#include "Pkt_Def.h"

/*****************************
	CONSTRUCTORS
******************************/
PktDef::PktDef()
{
	comPkt.header.pktCount = 0;
	comPkt.header.Drive = 0;
	comPkt.header.Status = 0;
	comPkt.header.Sleep = 0;
	comPkt.header.Arm = 0;
	comPkt.header.Claw = 0;
	comPkt.header.Ack = 0;
	comPkt.header.Padding = 0;
	comPkt.header.length = 0;
	comPkt.data = nullptr;
	comPkt.CRC = 0;
	RawBuffer = nullptr;
}

PktDef::PktDef(char* buffer)
{
	if (buffer != nullptr)
	{
		memcpy(&comPkt.header.pktCount, buffer, sizeof(int));

		comPkt.header.Drive = (buffer[4] & 1);
		comPkt.header.Status = ((buffer[4] >> 1) & 1);
		comPkt.header.Sleep = ((buffer[4] >> 2) & 1);
		comPkt.header.Arm = ((buffer[4] >> 3) & 1);
		comPkt.header.Claw = ((buffer[4] >> 4) & 1);
		comPkt.header.Ack = ((buffer[4] >> 5) & 1);
		comPkt.header.Padding = 0;

		memcpy(&comPkt.header.length, &buffer[1 + sizeof(int)], sizeof(char));
		int dataSize = comPkt.header.length - (HEADERSIZE + 1);

		if (comPkt.header.length > (HEADERSIZE + 1))
		{
			comPkt.data = new char[dataSize];

			for (int i = 0; i < dataSize; i++)
				memcpy(&comPkt.data[i], &buffer[HEADERSIZE + i], sizeof(char));

		}

		memcpy(&comPkt.CRC, &buffer[8], sizeof(char));
	}
}

/*****************************
	SETTERS
******************************/

void PktDef::SetCmd(CmdType passedCom)
{
	if (passedCom >= DRIVE && passedCom <= ACK)
	{
		if (passedCom != ACK)
		{
			comPkt.header.Drive = 0;
			comPkt.header.Ack = 0;
			comPkt.header.Status = 0;
			comPkt.header.Sleep = 0;
			comPkt.header.Arm = 0;
			comPkt.header.Claw = 0;
		}

		if (passedCom == DRIVE)
			comPkt.header.Drive = 1;
		else if (passedCom == SLEEP)
			comPkt.header.Sleep = 1;
		else if (passedCom == ARM)
			comPkt.header.Arm = 1;
		else if (passedCom == CLAW)
			comPkt.header.Claw = 1;
		else if (passedCom == ACK)
			comPkt.header.Ack = 1;
	}
}

void PktDef::SetBodyData(char* newData, int size)
{
	if (size != 0)
	{
		comPkt.data = new char[size];
		memset(comPkt.data, 0x00, 2);

		for (int i = 0; i < size; i++)
			memcpy(&comPkt.data[i], &newData[i], 1);
	}
	comPkt.header.length = HEADERSIZE + size + 1;
}

void PktDef::SetPktCount(int count)
{
	if (count > 0)
	{
		comPkt.header.pktCount += count;
	}
}

/*******************************
	GETTERS
********************************/

CmdType PktDef::GetCmd()
{
	if (comPkt.header.Drive == 1)
		return DRIVE;
	else if (comPkt.header.Sleep == 1)
		return SLEEP;
	else if (comPkt.header.Arm == 1)
		return ARM;
	else if (comPkt.header.Claw == 1)
		return CLAW;
	else
		return NACK;
}

bool PktDef::GetAck()
{
	return comPkt.header.Ack == 1;
}

bool PktDef::GetStatus()
{
	return comPkt.header.Status == 1;
}

int PktDef::GetLength()
{
	return (int)comPkt.header.length;
}

char* PktDef::GetBodyData()
{
	return comPkt.data;
}

int PktDef::GetPktCount()
{
	return comPkt.header.pktCount;
}

/*******************************
	CRC
********************************/

bool PktDef::CheckCRC(char* buffer, int size)
{
	this->CalcCRC();

	if (this->comPkt.CRC == buffer[size - 1])
		return true;
	else
		return false;
}

void PktDef::CalcCRC()
{
	char* ptr = (char*)&comPkt.header;
	char count = 0;
	int bodySize = comPkt.header.length - (HEADERSIZE + 1);

	for (int i = 0; i < HEADERSIZE; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			count += (ptr[i] >> j) & 1;
		}
	}

	if (comPkt.header.length > 7)
	{
		ptr = (char*)comPkt.data;

		for (int i = 0; i < bodySize; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				count += (ptr[i] >> j) & 1;
			}
		}
	}
	comPkt.CRC = count;
}

/********************************
	PACKET GENERATOR
*********************************/

char* PktDef::GenPacket()
{
	int size = this->comPkt.header.length;

	if (size <= 7)
	{
		RawBuffer = new char[HEADERSIZE + 1];
		memset(RawBuffer, 0x00, HEADERSIZE + 1);
		memcpy(RawBuffer, &comPkt.header, HEADERSIZE);
		RawBuffer[HEADERSIZE] = comPkt.CRC;
	}
	else
	{
		RawBuffer = new char[size];
		memset(RawBuffer, 0x00, size);
		memcpy(RawBuffer, &comPkt.header, HEADERSIZE);

		for (int i = 0; i < sizeof(MotorBody); i++)
			RawBuffer[i + HEADERSIZE] = comPkt.data[i];

		RawBuffer[size - 1] = comPkt.CRC;
	}
	return RawBuffer;
}
