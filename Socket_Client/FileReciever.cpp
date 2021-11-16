#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define _CRT_SECURE_NO_WARNINGS 1

#include "FileReciever.h"

#include <direct.h>
#include <stdio.h>
#include <io.h>
#include <filesystem>

#include<iostream>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

FileReciever::FileReciever(const std::string&IP, int port)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	Sock = socket(AF_INET, SOCK_STREAM, 0);

	Connect(IP, port);
}

int FileReciever::Connect(const std::string&IP, int port)
{
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(port);
	Addr.sin_addr.S_un.S_addr = inet_addr(IP.c_str());

	if (connect(Sock, (sockaddr*)&Addr, sizeof(sockaddr)) != SOCKET_ERROR)
	{
		cout << "failed.." << endl;
	}
	return 1;
}

FileReciever::~FileReciever()
{
	CloseSocket();
}

int FileReciever::RecieveFile(const char *path)
{
	std::cout << path << std::endl;
	
	std:string Unreal = "Unreal:";
	std::string Unreal_Filename;
	std::string Filename;
	//Filename = Hear();
	Unreal_Filename = Hear();
	//Filename=Filename.replace(,,)
	Filename = Unreal_Filename.replace(Unreal_Filename.find(Unreal), Unreal.length(), path);
	LOG(Filename.data());

	long long  FileSize = 0;
	recv(Sock, (CHAR*)&FileSize, sizeof(FileSize), 0);
	LOG(std::to_string(FileSize).data());

	std::string NewPath;
	NewPath = std::filesystem::path(Filename).parent_path().string();
	std::filesystem::create_directories(std::filesystem::path(NewPath));

	ofstream Fstream(Filename, ios::binary);
	if (Fstream.is_open())
	{
		Fstream.clear();

		for (;  Fstream.tellp() < FileSize;)
		{
			memset(CacheAry.data(), 0, CacheSize);

			int ReciveLen = Fstream.tellp();
			int CurrentRecvLen = 0;
			if ((FileSize - ReciveLen) < CacheSize)
			{
				CurrentRecvLen = recv(Sock, CacheAry.data(), FileSize - ReciveLen, 0);
			}
			else
			{
				CurrentRecvLen = recv(Sock, CacheAry.data(), CacheSize, 0);
			}

			Fstream.write(CacheAry.data(), CurrentRecvLen);

		}
		LOG(std::to_string(Fstream.tellp()).data());
	}

	return 0;
	
}

int FileReciever::Say(const std::string & message)
{
	memset(CacheAry.data(), 0, CacheSize);
	strcpy(CacheAry.data(), message.data());
	int send_len = send(Sock, CacheAry.data(), CacheSize, 0);
	return send_len;
}



int FileReciever::NewSay(const std::string & message)
{
	
	string s = message;
	Say(std::to_string(s.size()));
	int SendSize = 0;
	for (; ;)
	{
		if ((s.size() - SendSize) < CacheSize)
		{
			auto TempStr = s.substr(SendSize, s.size() - SendSize);
			send(Sock, (CHAR*)TempStr.data(), TempStr.size(), 0);
			break;
		}
		else
		{
			auto TempStr = s.substr(SendSize, CacheSize);
			SendSize += send(Sock, (CHAR*)TempStr.data(), TempStr.size(), 0);
		}
	}

	return s.size();
}

std::string FileReciever::Hear()
{
	int recv_len = recv(Sock , CacheAry.data(), CacheSize, 0);
	if (recv_len < 0)
	{
		cout << "Hear fail";
	}
	else
	{
		cout << CacheAry.data() << endl;
	}
	return CacheAry.data();
}


std::string FileReciever::HearMd5filesName()
{

	std::string Md5filesNum;
	Md5filesNum = FileReciever::Hear();
	int Ssize = atoi(Md5filesNum.c_str());
	int RecSize = 0;
	vector<char>StrContent;
	for (; ;)
	{
		memset(CacheAry.data(), 0, CacheSize);
		if ((Ssize - RecSize) < CacheSize)
		{
			recv(Sock, CacheAry.data(), Ssize - RecSize, 0);
			for (int Index = 0; Index < (Ssize - RecSize); Index++)
			{
				StrContent.emplace_back(CacheAry[Index]);
			}
			break;
		}
		else
		{
			RecSize += recv(Sock, CacheAry.data(), CacheSize, 0);
			for (int Index = 0; Index < CacheSize; Index++)
			{
				StrContent.emplace_back(CacheAry[Index]);
			}
			//sprintf(StrContent + RecSize - CacheSize, "%s", CacheAry.data());
		}
	}

	string s = "";
	std::vector<char>::iterator it = StrContent.begin();
	while (it != StrContent.end())
	{
		s += *it;
		it++;
	}

	//std::string Result(StrContent);
	return s;
}


void FileReciever::CloseSocket()
{
	closesocket(Sock);
	WSACleanup();
}