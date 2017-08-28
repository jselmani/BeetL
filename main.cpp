#include "MySocket.h"
#include "Pkt_Def.h"
#include <string>
#include <iomanip>
#include <thread>

bool ExeComplete = false;

void cmdThread(int portNum, std::string ip)
{
	MySocket comSocket(SocketType::CLIENT, ip, portNum, ConnectionType::TCP, 100);
	comSocket.ConnectTCP();

	PktDef txPkt;
	MotorBody bodyData;
	CmdType cmd;

	while (ExeComplete == false)
	{
		unsigned short int userCmd, dir = 0, dur = 0;
		char* txBuffer, rxBuffer[128];

		//query user for input
		std::cout << "DRIVE = 1\nSLEEP = 3\nARM = 4\nCLAW = 5" << std::endl;
		std::cout << "COMMAND: ";
		std::cin >> userCmd;
		std::cout << std::endl;

		txPkt.SetCmd((CmdType)userCmd);
		cmd = txPkt.GetCmd();

		if (cmd != SLEEP)
		{
			if (cmd == DRIVE)
				std::cout << "FORWARD = 1\nBACKWARD = 2\nRIGHT = 3\nLEFT = 4" << std::endl;
			else if (cmd == ARM)
				std::cout << "UP = 5\nDOWN = 6" << std::endl;
			else if (cmd == CLAW)
				std::cout << "OPEN = 7\nCLOSE = 8" << std::endl;

			std::cout << "Direction: ";
			std::cin >> dir;
			std::cout << std::endl;

			if (cmd == DRIVE)
			{
				std::cout << "Duration (in seconds): ";
				std::cin >> dur;
				std::cout << std::endl;
			}

			bodyData.direction = (char)dir;
			bodyData.duration = (char)dur;
			txPkt.SetBodyData((char*)&bodyData, sizeof(bodyData));
		}
		else if (cmd == SLEEP)
			txPkt.SetBodyData((char*)&bodyData, 0);

		txPkt.SetPktCount(1); //increment pktCount by 1
		txPkt.CalcCRC();

		txBuffer = new char[txPkt.GetLength()];
		txBuffer = txPkt.GenPacket();

		comSocket.SendData(txBuffer, txPkt.GetLength());
		comSocket.GetData(rxBuffer);

		PktDef rxPkt(rxBuffer);

		if (txPkt.GetCmd() == SLEEP && rxPkt.GetCmd() == SLEEP && rxPkt.GetAck())
		{
			comSocket.DisconnectTCP();
			ExeComplete = true;
		}
		else if (rxPkt.GetAck() == true)
			std::cout << "Command: " << rxPkt.GetCmd() << std::endl;
		else if (rxPkt.GetCmd() == NACK && rxPkt.GetAck() == false)
			std::cout << "Rejected. Try again." << std::endl;
	}
}

void telThread(int portNum, std::string ip)
{
	MySocket telSocket(SocketType::CLIENT, ip, portNum, ConnectionType::TCP, 100);
	telSocket.ConnectTCP();

	while (1)
	{
		int rxSize;
		char rxBuffer[100];
		char* ptr = rxBuffer;
		TelBody data;
		telSocket.GetData(rxBuffer);

		PktDef rxPkt(rxBuffer);
		bool result = rxPkt.CheckCRC(rxBuffer, rxPkt.GetLength());

		if (result)
		{
			if (rxPkt.GetStatus())
			{
				std::cout << std::endl;
				std::cout << "RAW Data: ";
				for (int i = 0; i < (int)rxPkt.GetLength(); i++)
					std::cout << std::hex
					<< std::setw(2) << (unsigned int)*(ptr++)
					<< ", ";

				std::cout << std::dec << std::endl;
				unsigned short int* uiptr = (unsigned short int*)rxPkt.GetBodyData();

				std::cout << "Sonar & Arm Data: " << std::endl;
				for (int i = 0; i < 2; i++)
					std::cout << *uiptr++ << ", ";

				std::cout << std::endl;
				ptr = rxPkt.GetBodyData() + (sizeof(unsigned short int) * 2);
				data.Drive = *ptr & 1;
				data.ArmUp = (*ptr >> 1) & 1;
				data.ArmDown = (*ptr >> 2) & 1;
				data.ClawOpen = (*ptr >> 3) & 1;
				data.ClawClosed = (*ptr >> 4) & 1;
				data.Padding = 0;

				std::cout << "Drive Bit: " << (bool)data.Drive << std::endl;

				if (data.ArmUp == 1 && data.ClawOpen == 1)
					std::cout << "Arm is Up, Claw is Open." << std::endl;
				else if (data.ArmUp == 1 && data.ClawClosed == 1)
					std::cout << "Arm is Up, Claw is Closed." << std::endl;
				else if (data.ArmDown == 1 && data.ClawOpen == 1)
					std::cout << "Arm is Down, Claw is Open." << std::endl;
				else if (data.ArmDown == 1 && data.ClawClosed == 1)
					std::cout << "Arm is Down, Claw is Closed." << std::endl;
			}
			else
			{
				std::cout << "Invalid Command." << std::endl;
			}
		}
		else
		{
			std::cout << "Invalid CRC." << std::endl;
		}
	}
}

int main()
{
	std::string comIP, telIP;
	int comPort, telPort;

	std::cout << "Please input Command IP: ";
	std::getline(std::cin, comIP);
	std::cout << std::endl;
	std::cout << "Please input Command Port: ";
	std::cin >> comPort;
	std::cin.ignore();
	std::cout << std::endl;

	std::cout << "Please input Telemetry IP: ";
	std::getline(std::cin, telIP);
	std::cout << std::endl;
	std::cout << "Please input Telemetry Port: ";
	std::cin >> telPort;


	std::thread(cmdThread, comPort, comIP).detach();
	std::thread(telThread, telPort, telIP).detach();

	while (!ExeComplete) {

	};
}
