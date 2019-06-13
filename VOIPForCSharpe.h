#pragma once
#pragma once
#include<string>
using namespace std;
extern "C" {
	//µÇÂ¼
	__declspec(dllexport) BOOL YPLogin(char* localuserid);
	//ºô½Ð
	__declspec(dllexport) int VoipCallPerson(char* targetId, char* str);
	//ÊÍ·Å
	__declspec(dllexport) int DeleteVoip();
}
