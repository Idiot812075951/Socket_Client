
#include <winsock2.h> 
#include <string>
#include <fstream>
#include <array>

#define LOG(logStr)\
{\
char LogAry[512] = {};\
sprintf_s(LogAry, "%s %d %s \n", __FUNCTION__, __LINE__, logStr);\
OutputDebugStringA(LogAry);\
}

struct FileName {//用来存储和传输文件名扩展名的结构体
	char Fname[64];
	int len;
};

class FileReciever 
{
public:

	FileReciever(const std::string&IP, int port);

	~FileReciever();

	int Connect(const std::string&IP, int port);

	int RecieveFile(const char *path);

	void CloseSocket();

	int Say(const std::string & message);

	int NewSay(const std::string & message);

	std::string Hear();

	std::string HearMd5filesName();

private:

	static const int CacheSize = 64;//512
	//当传输容器最后一个索引的时候，会乱码

	std::array<char, CacheSize>CacheAry;

	SOCKET Sock;

	sockaddr_in Addr;

};
