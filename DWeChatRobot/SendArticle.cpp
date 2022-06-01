#include "pch.h"

// 发送文章CALL1偏移
#define SendArticleCall1Offset 0x54328A10 - 0x54270000
// 发送文章CALL2偏移
#define SendArticleCall2Offset 0x5465D5E0 - 0x54270000
// 发送文章CALL3偏移
#define SendArticleCall3Offset 0x54377EB0 - 0x54270000
// 发送文章CALL4偏移
#define SendArticleCall4Offset 0x5465D7B0 - 0x54270000
// 发送文章CALL参数偏移
#define SendArticleParamOffset 0x565F3FE4 - 0x54270000

// 清空缓存CALL1偏移
#define SendArticleClearCacheCall1Offset 0x54916450 - 0x54270000
// 清空缓存CALL2偏移
#define SendArticleClearCacheCall2Offset 0x54327720 - 0x54270000

/*
* 外部调用时传递的参数结构
* wxid：接收人的保存地址
* title：文章标题的保存地址
* abstract：文章摘要的保存地址
* url：文章链接的保存地址
*/
struct SendArticleStruct {
	DWORD wxid;
	DWORD title;
	DWORD abstract;
	DWORD url;
};

/*
* 供外部调用的发送文章消息接口
* lparameter：SendArticleStruct类型结构体指针
* return：void
*/
VOID SendArticleRemote(LPVOID lparameter) {
	SendArticleStruct* sas = (SendArticleStruct*)lparameter;
	wchar_t* wxid = (wchar_t*)sas->wxid;
	wchar_t* title = (wchar_t*)sas->title;
	wchar_t* abstract = (wchar_t*)sas->abstract;
	wchar_t* url = (wchar_t*)sas->url;
	SendArticle(wxid,title,abstract,url);
}

/*
* 获取自己的wxid保存地址
* return：DWORD，个人wxid保存地址
*/
DWORD GetSelfWxIdAddr() {
	DWORD baseAddr = GetWeChatWinBase() + 0x236307C;
	char wxidbuffer[0x100] = { 0 };
	DWORD SelfWxIdAddr = 0x0;
	sprintf_s(wxidbuffer, "%s", (char*)baseAddr);
	if (strlen(wxidbuffer) < 0x6 || strlen(wxidbuffer) > 0x14)
	{
		SelfWxIdAddr = *(DWORD*)baseAddr;
	}
	else
	{
		SelfWxIdAddr = baseAddr;
	}
	return SelfWxIdAddr;
}

/*
* 发送文章消息的具体实现
* wxid：消息接收人wxid
* title：文章标题
* abstract：文章摘要
* url：文章链接
* return：BOOL，成功返回`1`，失败返回`0`
*/
BOOL __stdcall SendArticle(wchar_t* wxid,wchar_t* title, wchar_t* abstract, wchar_t* url) {
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD SendArticleCall1 = WeChatWinBase + SendArticleCall1Offset;
	DWORD SendArticleCall2 = WeChatWinBase + SendArticleCall2Offset;
	DWORD SendArticleCall3 = WeChatWinBase + SendArticleCall3Offset;
	DWORD SendArticleCall4 = WeChatWinBase + SendArticleCall4Offset;

	DWORD SendArticleParam = WeChatWinBase + SendArticleParamOffset;

	DWORD SendArticleClearCacheCall1 = WeChatWinBase + SendArticleClearCacheCall1Offset;
	DWORD SendArticleClearCacheCall2 = WeChatWinBase + SendArticleClearCacheCall2Offset;
	// 自己的wxid，发送者
	char* sselfwxid = (char*)GetSelfWxIdAddr();
	wchar_t* wselfwxid = new wchar_t[strlen(sselfwxid) + 1];
	MultiByteToWideChar(CP_ACP, 0, sselfwxid, -1, wselfwxid, strlen(sselfwxid) + 1);
	// 构造xml数据
	wchar_t* xmlbuffer = new wchar_t[0x2000];
	ZeroMemory(xmlbuffer, 0x2000 * 2);
	swprintf_s(xmlbuffer,0x2000, (wchar_t*)L"<msg>\n    <fromusername>%ws</fromusername>\n    <scene>0</scene>\n    <commenturl></commenturl>\n    <appmsg appid=\"\" sdkver=\"0\">\n        <title>%ws</title>\n        <des>%ws</des>\n        <action>view</action>\n        <type>5</type>\n        <showtype>0</showtype>\n        <content></content>\n        <url>%ws</url>\n        <dataurl></dataurl>\n        <lowurl></lowurl>\n        <lowdataurl></lowdataurl>\n        <recorditem>\n            <![CDATA[]]>\n        </recorditem>\n        <thumburl></thumburl>\n        <messageaction></messageaction>\n        <extinfo></extinfo>\n        <sourceusername></sourceusername>\n        <sourcedisplayname></sourcedisplayname>\n        <commenturl></commenturl>\n        <appattach>\n            <totallen>0</totallen>\n            <attachid></attachid>\n            <emoticonmd5></emoticonmd5>\n            <fileext></fileext>\n            <aeskey></aeskey>\n        </appattach>\n        <weappinfo>\n            <pagepath></pagepath>\n            <username></username>\n            <appid></appid>\n            <appservicetype>0</appservicetype>\n        </weappinfo>\n        <websearch />\n    </appmsg>\n    <appinfo>\n        <version>1</version>\n        <appname>Window wechat</appname>\n    </appinfo>\n</msg>",
		wselfwxid,title,abstract,url);

	DWORD sendtype = 0x5;
	WxBaseStruct pSender(wselfwxid);
	char nullbuffer[0x1C] = { 0 };
	WxBaseStruct pXml(xmlbuffer);
	WxBaseStruct pReceiver(wxid);
	WxString imgbuffer = { 0 };
	WxString nullStruct = { 0 };
	char buffer[0xFF0] = { 0 };
	DWORD isSuccess = 0x0;
	__asm {
		pushad;
		pushfd;
		lea ecx, buffer;
		call SendArticleCall1;
		mov eax, [sendtype];
		push eax;
		lea eax, nullbuffer;
		lea edx, pSender;
		push eax;
		lea eax, imgbuffer;
		push eax;
		lea eax, pXml;
		push eax;
		lea edi, pReceiver;
		push edi;
		lea ecx, buffer;
		call SendArticleCall2;
		add esp, 0x14;
		lea eax, nullStruct;
		push eax;
		lea ecx, buffer;
		call SendArticleCall3;
		mov dl, 0x0;
		lea ecx, buffer;
		push SendArticleParam;
		push SendArticleParam;
		call SendArticleCall4;
		mov isSuccess, eax;
		add esp, 0x8;
		lea ecx, buffer;
		call SendArticleClearCacheCall1;
		lea ecx, buffer;
		call SendArticleClearCacheCall2;
		popfd;
		popad;
	}
	delete[] xmlbuffer;
	xmlbuffer = NULL;
	delete[] wselfwxid;
	wselfwxid = NULL;
	return (isSuccess == 0x1);
}