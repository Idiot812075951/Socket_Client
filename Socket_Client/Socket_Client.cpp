#pragma once

#include <stdio.h>
#include <io.h>

#include <iostream>
#include <set>
#include <thread>
#include <string>

#include <map>
#include"FileReciever.h"
//#include <windows.h>
#include <TlHelp32.h>

#include <filesystem>
#include "Md5file.h"

#pragma comment(lib,"Urlmon.lib") //加入链接库
#pragma comment(lib,"ws2_32.lib")


using namespace std;

void initialization();
void invokeBat(LPCWSTR batname);

//删除服务
BOOL DeliteSampleService(LPCWSTR ServiceName)
{
	SERVICE_STATUS status;

	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  // 打开服务控制管

													   //理数据库，并返回服务控制管理数据库的句柄

	if (schSCManager == NULL)

	{

		return FALSE;

	}

	SC_HANDLE schService = OpenService(schSCManager, ServiceName,

		SERVICE_ALL_ACCESS | DELETE);   // 获得服务句柄

	if (schService == NULL)

	{

		return FALSE;

	}

	QueryServiceStatus(schService, &status);  // 获得服务的当前状态

	if (status.dwCurrentState != SERVICE_STOPPED)   // 如果服务不处于停止状态，则将其状态设置为

													//停止状态
		ControlService(schService, SERVICE_CONTROL_STOP, &status);

	DeleteService(schService);   // 删除服务
	CloseServiceHandle(schSCManager);  // 关闭服务句柄
	CloseServiceHandle(schService);

	return TRUE;
}
//启动服务
BOOL StartSampleService(LPCWSTR ServiceName)
{
	SERVICE_STATUS status;

	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  // 打开服务控制管

													   //理数据库，并返回服务控制管理数据库的句柄
	if (schSCManager == NULL)
	{
		return FALSE;
	}

	SC_HANDLE schService = OpenService(schSCManager, ServiceName,

		SERVICE_ALL_ACCESS | DELETE);   // 获得服务句柄
	if (schService == NULL)
	{
		return FALSE;
	}

	QueryServiceStatus(schService, &status);   // 获得服务的当前状态

	if (status.dwCurrentState = SERVICE_STOPPED)   // 如果服务处于停止状态，则将其状态设置为启动

													//状态
		StartService(schService, 0, NULL);   //启动服务

	CloseServiceHandle(schSCManager);  // 关闭服务句柄
	CloseServiceHandle(schService);

	return TRUE;
}
//安装服务
BOOL CreateSampleService(LPCWSTR ServiceName)
{
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  // 打开服务控制管

													   //理数据库，并返回服务控制管理数据库的句柄
	if (schSCManager == NULL)
	{
		return FALSE;
	}
	SC_HANDLE schService = CreateService(schSCManager, ServiceName, ServiceName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		L"C:\\Windows\\SysWOW64\\RegProtect\\RegProtect.exe",
		NULL, NULL, NULL, NULL, NULL);   //安装服务
	if (schService == NULL)
	{
		return FALSE;
	}
	CloseServiceHandle(schSCManager);   //关闭服务句柄
	CloseServiceHandle(schService);

	StartSampleService(ServiceName);  //启动服务
	return TRUE;
}

void download(string dourl, string a);

void SplitStringToVector(const string& s, vector<string>& V, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		V.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		V.push_back(s.substr(pos1));
}





set<string> extractFiles(const string &folder)
{
	set<string> Result;

	if (!exists(filesystem::path(folder)))
	{  //目录不存在直接返回
		return Result;
	}
	auto begin = filesystem::recursive_directory_iterator(folder); //获取文件系统迭代器
	auto end = filesystem::recursive_directory_iterator();    //end迭代器 

	std::vector<string> Vec_del;
	for (auto it = begin; it != end; it++)
	{
		const string spacer(it.depth() * 2, ' ');  //这个是用来排版的空格
		auto& entry = *it;
		if (filesystem::is_regular_file(entry))
		{
			Result.insert(entry.path().string());
		}
		else if (filesystem::is_directory(entry))
		{
			auto Temp = extractFiles(entry.path().string());
			for (auto Iter : Temp)
			{
				Result.insert(Iter);
			}
			if (Temp.size() == 0)
			{
				string a = entry.path().string();
				Vec_del.push_back(a);
				
			}
		}
	}

	for (auto i : Vec_del)
	{
		filesystem::remove(i);
	}

	return Result;
}

void UpdateFIles(string client_path,string server_path,string listen_ip) 
{
	int port = 3300;
	//std::string IP("192.168.0.73");
	//FileReciever fr(IP, port);
	FileReciever fr(listen_ip, port);
	fr.Say(server_path);
	//注意，这个是路径，是对比文件差异的，下面那个路径是接收，好像这两个是同一路径
// 	string folder = "D://AutoUpdateFolder";
// 	string unreal = "Unreal:";
	string folder = client_path;
	string unreal = "Unreal:";
	set<string>sever_files = extractFiles(folder);


	

	set<string>::iterator it = sever_files.begin();


	string s = "";
	while (it != sever_files.end())
	{
		string tmps = *it;
		tmps = tmps.replace(tmps.find(folder), folder.length(), unreal);
		s += tmps+";";
		it++;
	}
	if (sever_files.size() > 0)
		fr.NewSay(s);
	else
		fr.NewSay("0");

	string Md5filesName = fr.HearMd5filesName();
	string s1 = "";
	if (Md5filesName=="0")
	{
		s1 = "1";
	}
	else
	{
		vector<string> Md5List;
		string sss = "1;2;3;";
		vector<string> V_test;
		SplitStringToVector(Md5filesName, Md5List, ";");
		SplitStringToVector(sss, V_test,";");


		vector<string>::iterator itt = Md5List.begin();
		while (itt != Md5List.end())
		{
			string tmps = *itt;
			tmps = tmps.replace(tmps.find(unreal), unreal.length(), folder);
			//格式： 1.txt::md5value1；2.txt::md5value2；
			string md5svalue = getFileMD5(tmps);
			string path = tmps.replace(tmps.find(folder), folder.length(), unreal);
			s1 += path + "::" + md5svalue + ";";
			itt++;
		}
	}
		//接下来就是发送md5值过去
		//然后考虑，如果md5不相等，该怎么做
	fr.NewSay(s1);


	//注意，这个是接收文件夹目录
	char p[] = "D://AutoUpdateFolder";

	int RecieveFilesNum;



	string IsTransfromFiles = fr.Hear();

	if (IsTransfromFiles.compare("IsTransformFiles") == 0)
	{
		//有文件发送过来
		string TransformFilesNum = fr.Hear();

		int i = atoi(TransformFilesNum.c_str());

		for (int i = 0; i < atoi(TransformFilesNum.c_str()); i++)
		{
			fr.RecieveFile(p);
		}

		const auto EndRecvFile = fr.Hear();
		if (EndRecvFile == "EndRecvFile")
		{

		}
	}
	else
	{
		//没有文件
	}
	//HearMd5filesName 这个函数其实就是NewHear()
	string s_delfiles = fr.HearMd5filesName();
	//Unreal:\Client - 副本 (2).txt;Unreal:\Client - 副本 (3).txt;Unreal:\Client - 副本 (4).txt;Unreal:\Client - 副本 (5).txt;Unreal:\Client - 副本.txt;

	vector<string> delfiles;
	SplitStringToVector(s_delfiles, delfiles, ";");
	for (auto &i : delfiles)
	{
		i = i.replace(i.find(unreal), unreal.length(), folder); 

	}

	for (auto i : delfiles)
	{
		filesystem::remove(i);
	}
	
	

}


bool traverseProcesses(std::map<wstring, int>& _nameID)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		std::cout << "CreateToolhelp32Snapshot Error!" << std::endl;;
		return false;
	}

	BOOL bResult = Process32First(hProcessSnap, &pe32);

	int num(0);

	while (bResult)
	{

		std::wstring name = pe32.szExeFile;

		int id = pe32.th32ProcessID;

		_nameID.insert(std::pair<wstring, int>(name, id)); //字典存储
		bResult = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);

	return true;
}


bool KillProcess(DWORD dwPid)
{
	printf("Kill进程Pid = %d\n", dwPid);
	//getchar();
	//关闭进程
	HANDLE killHandle = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION |   // Required by Alpha
		PROCESS_CREATE_THREAD |   // For CreateRemoteThread
		PROCESS_VM_OPERATION |   // For VirtualAllocEx/VirtualFreeEx
		PROCESS_VM_WRITE,             // For WriteProcessMemory);
		FALSE, dwPid);
	if (killHandle == NULL)
		return false;
	TerminateProcess(killHandle, 0);
	return true;
}


int main(int argc, char *argv[])//有几个参数， 参数的内容是
{
	//艹，argv[0] 是他自己exe的全路径
	string client_path = argv[1];
	string server_path = argv[2];
	string listen_ip = argv[3];
	const string mode = argv[4];
	int int_mode = atoi(mode.c_str());
	//int a = Integer.parseInt(str);

	cout << client_path << endl;
	cout << server_path << endl;
	cout << listen_ip << endl;

	//不能再case里面定义变量，艹了，只有写在外面
	//这是为了关闭渲染程序
	std::map<wstring, int> _nameID;
	if (!traverseProcesses(_nameID)) {
		cout << "Start Process Error!" << endl;
	}
	auto pro_ptr = _nameID.find(L"WeChat.exe");
	auto pro_id = pro_ptr->second;

	switch (int_mode)
	{
	//0代表普通文件，不需要额外的操作
	case 0:
		UpdateFIles(client_path, server_path, listen_ip);
		break;
	//1代表是网页相关文件，需要 关闭/开启 IIS服务
	case 1:
		system("iisreset/stop");
		Sleep(1000);
		//UpdateFIles(client_path, server_path, listen_ip);
		cout << "asdasd";
		system("iisreset/start");
		break;
	//2代表是渲染相关文件，需要 关闭/开启 渲染程序
	case 2:
		if (pro_ptr!=_nameID.end())
		{
			KillProcess(pro_id);
		}

		UpdateFIles(client_path, server_path, listen_ip);
		break;
	//3代表是数据库相关文件，需要备份
	case 3:
		//备份操作,直接调用BAT就行
		//UpdateFIles(client_path, server_path, listen_ip);
		LPCWSTR batName = L"D:\\AutoUpdate\\Server\\Socket_Server\\Socket_Server\\1.bat";
		invokeBat(batName);
		break;
	}
	return 0;
}




void initialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}
	//填充服务端地址信息

}

LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);


	return wcstring;
}


void download(string dourl, string a)
{
	LPCWSTR url = stringToLPCWSTR(dourl);
	printf("下载链接: %S\n", url);
	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	LPCWSTR savepath = stringToLPCWSTR(a);
	wsprintf(path, savepath, path);
	printf("保存路径: %S\n", path);
	HRESULT res = URLDownloadToFile(NULL, url, path, 0, NULL);
	if (res == S_OK)
	{
		printf("下载完毕\n");
	}
	else if (res == E_OUTOFMEMORY)
	{
		printf("接收长度无效，或者并未定义\n");
	}
	else if (res == INET_E_DOWNLOAD_FAILURE)
	{
		printf("URL无效\n");
	}
	else
	{
		printf("未知错误\n", res);
	}
}


void invokeBat(LPCWSTR batname)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	if (CreateProcess(batname,// RUN_TEST.bat位于工程所在目录下
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NO_WINDOW,// 这里不为该进程创建一个控制台窗口
		NULL,
		NULL,
		&si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);// 等待bat执行结束
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return ;
}


