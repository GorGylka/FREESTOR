#include "network.hpp"
#include <pspsdk.h>

#include <pspdebug.h>
#include <psputility.h>
#include <pspdisplay.h>

#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <psphttp.h>

#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include <curl/curl.h>

#define printf pspDebugScreenPrintf

int netDialogActive = -1;

int progress_func(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
int percent = 0;
percent = ( NowDownloaded / TotalToDownload * 100);
std::stringstream ssss;
ssss << percent;
const char* str = ssss.str().c_str();
//printf(str);
pspDebugScreenSetXY(34,15);
pspDebugScreenPrintf(str);
pspDebugScreenPutChar(252,120,0xFFFFFF, '%');

percent = (percent + 190);
pspDebugScreenPutChar(percent,100,0x4444FF, '>');
pspDebugScreenPutChar(184,100,0xFFFFFF, '[');
pspDebugScreenPutChar(296,100,0xFFFFFF, ']');
return 0;
}


int curlDownload(std::string &url, std::string file)
{
  std::ofstream saveFile(file.c_str(), std::ios::binary);
  if (!saveFile.is_open())
  {
    printf("Could not open file %s for writing!\n", file.c_str());
    return -1;
  }

  CURL *curl = curl_easy_init();

  if (!curl)
  {
    printf("Could not initialize cURL!\n");
    return -1;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &saveFile);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
  curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK)
  {
    printf("cURL error: %s\n", curl_easy_strerror(res));
    std::cout << "URL: " << url << std::endl;
    std::cout << "File: " << file << std::endl;
    std::cout << "Error code: " << res << std::endl;
    std::cout << "Error string: " << curl_easy_strerror(res) << std::endl;
    return -1;
  }

  curl_easy_cleanup(curl);

  saveFile.close();

  return 0;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  std::ofstream *out = (std::ofstream *)stream;
  out->write((char *)ptr, size * nmemb);
  return size * nmemb;
}

void loadNetworkingLibs()
{
  int rc = sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
  if (rc < 0)
    printf("net common didn't load.\n");
  rc = sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
  if (rc < 0)
    printf("inet didn't load.\n");
}

void httpInit()
{
  pspDebugScreenPrintf("Loading module SCE_SYSMODULE_HTTP\n");
  sceUtilityLoadNetModule(PSP_NET_MODULE_HTTP);

  pspDebugScreenPrintf("Running sceHttpInit\n");
  sceHttpInit(4 * 1024 * 1024);
}

void httpTerm()
{
  pspDebugScreenPrintf("Running sceHttpTerm\n");
  sceHttpEnd();

  pspDebugScreenPrintf("Unloading module SCE_SYSMODULE_HTTP\n");
  sceUtilityUnloadNetModule(PSP_NET_MODULE_HTTP);
}

int goOnline()
{
  sceNetInit(128 * 1024, 42, 4 * 1024, 42, 4 * 1024);
  sceNetInetInit();
  sceNetApctlInit(0x8000, 48);
  sceNetResolverInit();
  if (!netDialog())
  {
    printf("Could not access networking dialog! %d", 30000);
    stopNetworking();
    return 1;
  }
  httpInit();
  return 0;
}

pspUtilityNetconfData data;

int netDialog()
{
  memset(&data, 0, sizeof(data));
  data.base.size = sizeof(data);
  data.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
  data.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
  data.base.graphicsThread = 17;
  data.base.accessThread = 19;
  data.base.fontThread = 18;
  data.base.soundThread = 16;
  data.action = PSP_NETCONF_ACTION_CONNECTAP;

  netDialogActive = -1;
  int result = sceUtilityNetconfInitStart(&data);
  printf("sceUtilityNetconfInitStart: %08x\n", result);
  if (result < 0)
  {
    data.base.size = sizeof(pspUtilityNetconfData) - 12;
    result = sceUtilityNetconfInitStart(&data);
    printf("sceUtilityNetconfInitStart again: %08x\n", result);
    if (result < 0)
      return 0;
  }
  netDialogActive = 0;

  return 1;
}

// returns -1 on quit, 0 on active, and 1 on success
int drawNetDialog()
{
  int done = 0;

  switch (sceUtilityNetconfGetStatus())
  {
  case PSP_UTILITY_DIALOG_NONE:
    printf("None\n");
    break;
  case PSP_UTILITY_DIALOG_INIT:
    break;
  case PSP_UTILITY_DIALOG_VISIBLE:
    sceUtilityNetconfUpdate(1);
    break;
  case PSP_UTILITY_DIALOG_QUIT:
    pspDebugScreenSetBackColor(0x000000);
	  pspDebugScreenSetTextColor(0xFFFFFF);
	  pspDebugScreenClear();
    printf("NetDialog was quit.\n");
    sceUtilityNetconfShutdownStart();
    done = -1;
    break; // cancelled??
  case PSP_UTILITY_DIALOG_FINISHED:
    printf("NetDialog completed successfully.\n");
    sceUtilityNetconfShutdownStart();
    done = 1;
    break;
  default:
    printf("NetconfGetStatus: %08x\n", sceUtilityNetconfGetStatus());
    break;
  }

  return done;
}

void startNetworking()
{
  printf("Loading Netowrk libs...\n");
  loadNetworkingLibs();
  printf("Initing networking\n");
  goOnline();

  printf("Starting dialog\n");
  while (netDialogActive != 1)
  {
    netDialogActive = drawNetDialog();

    if (netDialogActive == -1)
    {
      printf("Dialog was quit.\n");
      break;
    }

    sceDisplayWaitVblankStart();
  }

  printf("Dialog finished.\n");

  atexit(stopNetworking);
}

void stopNetworking()
{
  printf("Shutting down networking.\n");
  httpTerm();
  sceNetResolverTerm();
  sceNetApctlTerm();
  sceNetInetTerm();
  sceNetTerm();
}