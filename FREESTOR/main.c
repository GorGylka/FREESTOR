
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <stdlib.h>
#include <string.h>
#include <psploadexec_kernel.h>
#include <psputility.h>
#include <pspgu.h>
#include <psprtc.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <psputility_netmodules.h>
#include <psputility_netconf.h>
#include <psputility_sysparam.h>
#include <stdio.h>
#include <pspkerneltypes.h>

#include <systemctrl.h>
#include <systemctrl_se.h>

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("Download", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int temp1 = 0;
long space_free;
int triangle_pressed = 0;
int square_pressed = 0;
int gamecounter = 1;
int ps1_choose =0;
char download_url[300];
char extraline[300];
char free_space[20];
char tempstr[10];
const char* Choose_database = "important_files/All_games_sort_by_Size.tsv";

typedef struct sDevCtl
{
  s32 max_clusters;
  s32 free_clusters;
  s32 max_sectors;  // ???
  s32 sector_size;
  s32 sector_count;
} sDevCtl;

typedef struct sDevCommand
{
  sDevCtl * p_dev_inf;
} sDevCommand;


long
psp_get_memory_stick_free()
{
  sDevCtl     dev_ctl;
  sDevCommand command;

  command.p_dev_inf = &dev_ctl;

  if ( sceIoDevctl( "ef0:", 0x02425818, &command, sizeof( sDevCommand ), NULL, 0 ) == SCE_KERNEL_ERROR_OK ) {
    return ((long long)dev_ctl.free_clusters * (long long)dev_ctl.sector_count * (long long)dev_ctl.sector_size) / (1024 * 1024);
  }
  return -1;
}








void drawUI()
      {
      pspDebugScreenSetTextColor(0x00DDDD);
      pspDebugScreenSetXY(0,31);
      pspDebugScreenPrintf("____________________________________________________________________");
      pspDebugScreenSetXY(0,32);
      pspDebugScreenPrintf("UP,DOWN=select game  X=install game TRNGL=sort games");
      pspDebugScreenSetTextColor(0xDDDD00);
      pspDebugScreenSetXY(54,32);
      pspDebugScreenPrintf(free_space);
      pspDebugScreenSetXY(61,32);
      pspDebugScreenPrintf("MB FREE");
      pspDebugScreenSetXY(0,33);
      pspDebugScreenSetTextColor(0x00DDDD);
      pspDebugScreenPrintf("L,R=page next/previous []=credits"); 
      pspDebugScreenSetTextColor(0xFFFFFF);
      pspDebugScreenSetXY(59,33);
      pspDebugScreenPrintf(tempstr);
      pspDebugScreenSetXY(63,33);
      pspDebugScreenPrintf("/1599"); 
      pspDebugScreenSetTextColor(0x00DDDD);
      if (ps1_choose == 12 || ps1_choose == 13 )  
      {
       pspDebugScreenSetTextColor(0xFFFFFF);
      pspDebugScreenSetXY(63,33);
      pspDebugScreenPrintf("/1127"); 
      pspDebugScreenSetTextColor(0x00DDDD);     
      }
      

      pspDebugScreenPutChar(0,8,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,16,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,24,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,32,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,40,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,48,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,56,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,64,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,72,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,80,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,88,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,96,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,104,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,112,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,120,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,128,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,136,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,144,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,152,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,160,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,168,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,176,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,184,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,192,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,200,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,208,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,216,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,224,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,232,0xFFFFFF, '>');
      pspDebugScreenPutChar(0,240,0xFFFFFF, '>');
      pspDebugScreenSetTextColor(0xFFFFFF);
      pspDebugScreenSetXY(0,0);
      pspDebugScreenSetTextColor(0x00DDDD);
      pspDebugScreenPrintf("                           -= FREESTOR =-");
      pspDebugScreenSetTextColor(0xFFFFFF);
      }

void DrawTable() {
	
      
      int xy = 1;
      FILE* file  = fopen(Choose_database,"r");
      fgets (download_url, 300, file); 
	pspDebugScreenSetXY(0,1);
	for (int i=0; i < temp1 ;i++) {
      fgets (download_url, 300, file);
      }
            for (int i=0; i < 30 ;i++)
            {
            memset(extraline, '\0', sizeof(extraline));
            fgets (download_url, 300, file);
            char* pos = strstr(download_url,"	"); 
            memcpy(extraline,download_url,pos-download_url);
            pos++;
            pspDebugScreenSetXY(2,xy);
            extraline[40] = 0; 
            pspDebugScreenPrintf(extraline);  //Print PA-TA-TON for example 
            memset(extraline, '\0', sizeof(extraline));
            char* pos1 = strstr(pos,"	"); 
            memcpy(extraline,pos,pos1-pos);
            pos1++;
            pspDebugScreenSetXY(45,xy);
            pspDebugScreenPrintf(extraline);  //minis for example
            memset(extraline, '\0', sizeof(extraline));
            pos = strstr(pos1,"	"); 
            memcpy(extraline,pos1,pos-pos1);
            pos++;
            pspDebugScreenSetXY(56,xy);
            pspDebugScreenPrintf(extraline);  //EU for example
            memset(extraline, '\0', sizeof(extraline));
            pos1 = strstr(pos,"	"); 
            memcpy(extraline,pos,pos1-pos);
            pos1++;
            

            //int size = strlen(pos1);
           // pos1[size-1] = '\0';

            pos1[strlen(pos1)-2] = '\0';
            pspDebugScreenSetXY(60,xy);
            pspDebugScreenPrintf(pos1);       //999mb for example

            pspDebugScreenPrintf(" MB"); 
            
            xy++;
            }

	xy = 1;
	fclose(file);
}

int start_download(char *url , char *path , char* flag);
int sceDisplaySetHoldMode(int a0);

static unsigned int __attribute__((aligned(16))) gu_list[262144];

//sub_00000280
static int InitNetModules()
{
	int res;
	if((res = sceNetInit( 0x20000 , 42 , 0, 42 , 0)) >=0) {
		if((res = sceNetInetInit()) >= 0) {
			if((res = sceNetResolverInit()) >= 0) {
				res = sceNetApctlInit( 0x5400 , 0x30 );
			}
		}
	}

	return res;
}

//sub_000002E4
static int LoadNetModules()
{
	int res;
	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_COMMON)) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading pspnet.prx.\n" , res);
		return res;
	}

	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_INET )) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading pspnet_inet.prx.\n" , res);
		return res;
	}

	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_PARSEURI )) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading parseuri.\n" , res);
		return res;
	}

	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_PARSEHTTP )) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading parsehttp.\n" , res);
		return res;
	}

	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_HTTP )) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading libhttp.prx.\n" , res);
		return res;
	}

	return 0;
}

static void drawStuff(u32 color)
{
	sceGuStart(0 , &gu_list);
	sceGuClearColor( color);
	sceGuClearDepth(0);
	sceGuClear(5);
	sceGuFinish();
	sceGuSync(0 , 0);
}

//sub_00000400
int ConnectInternet()
{
	int lang , button;
	pspUtilityNetconfData NetData;

	sceUtilityGetSystemParamInt( PSP_SYSTEMPARAM_ID_INT_LANGUAGE , &lang);
	sceUtilityGetSystemParamInt( 9 , &button);

	memset( &NetData, 0 ,sizeof(pspUtilityNetconfData));
	NetData.base.size			= sizeof(pspUtilityNetconfData);
	NetData.base.language		= lang;
	NetData.base.buttonSwap		= button;
	NetData.base.graphicsThread	= 17;
	NetData.base.accessThread	= 19;
	NetData.base.fontThread		= 18;
	NetData.base.soundThread	= 16;
	NetData.action= PSP_NETCONF_ACTION_CONNECTAP;

	sceUtilityNetconfInitStart(&NetData);
	int done=0;

	while(1) {
		drawStuff( 0xFF000000 );

		switch(sceUtilityNetconfGetStatus()) {
			//case PSP_UTILITY_DIALOG_NONE:
			//	break;

			case PSP_UTILITY_DIALOG_VISIBLE://2
				sceUtilityNetconfUpdate(1);
				break;

			case PSP_UTILITY_DIALOG_QUIT://3
				sceUtilityNetconfShutdownStart();
				done = 1;
				break;
				
			//case PSP_UTILITY_DIALOG_FINISHED:
			//	break;

			default:
				break;
		}

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
		
		if(done)
			break;
	}

	return 1;
}

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

static void setupGu()
{
	sceGuInit();
   	sceGuStart(GU_DIRECT, gu_list );
   	sceGuDrawBuffer(GU_PSM_8888,(void*)0, BUF_WIDTH );
   	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
   	sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
   	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
   	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
   	sceGuDepthRange(0xc350,0x2710);
   	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
   	sceGuEnable(GU_SCISSOR_TEST);
   	sceGuDepthFunc(GU_GEQUAL);
   	sceGuEnable(GU_DEPTH_TEST);
   	sceGuFrontFace(GU_CW);
   	sceGuShadeModel(GU_SMOOTH);
  	sceGuEnable(GU_CULL_FACE);
   	sceGuEnable(GU_CLIP_PLANES);
   	sceGuFinish();
   	sceGuSync(0,0);

   	sceDisplayWaitVblankStart();
   	sceGuDisplay(GU_TRUE);
}





int C_downloader()
{	char download_url2[256];
	FILE* file  = fopen("url.txt","r");
	//getline(file,download_url);
	while (!feof(file)) {
	fgets (download_url2, 256, file);

	}
	fclose(file);

	u64 tick;
	SceKernelUtilsMt19937Context ctx;

	

	sceRtcGetCurrentTick(&tick);
	sceKernelUtilsMt19937Init(&ctx, tick);
	//u32 rand_val = sceKernelUtilsMt19937UInt(&ctx);


	//pspDebugScreenSetTextColor( (0x00F0F0F0 & rand_val) ^ (0x000F0F0F & rand_val) << 4 );
	pspDebugScreenSetTextColor(0xFFFFFF);
	LoadNetModules();
	InitNetModules();

	setupGu();
	drawStuff( 0xFF0000CC );
	ConnectInternet();

	sceKernelDelayThread(100);

	pspDebugScreenInit();
	pspDebugScreenClear();

//	int r = sub_0000071C( download_url , "ef0:/PSP/GAME/UPDATE/500.PBP" , NULL );
	int r = start_download( download_url2 , "ef0:/PSP/GAME/FREESTOR/pkg/game.pkg" , "PSPUpdate-agent/1.0.0 libhttp/1.0.0" );
//	int r = start_download( download_url , SAVE_PATH , "PSPUpdate-agent/1.0.0 libhttp/1.0.0" );
	
/*	if(r != 0){
		printf("uFetchFile: Error 0x%08X\n",r);
            //printf("maybe ef0:/pkg folder is missing???");
 
		sceKernelDelayThread( 0x4C4B40 ); 
		sceKernelExitGame(); 
		return sceKernelSleepThread(); 
	} */


	struct SceKernelLoadExecVSHParam param;
      pspDebugScreenSetTextColor(0x00DDDD);
	printf("\nDOWNLOAD COMPLETE! NOW INSTALLING...\n");
      
      pspDebugScreenSetTextColor(0xFFFFFF);
	sceDisplaySetHoldMode(1);

	memset( &param , 0 , sizeof(param) );
	param.size= sizeof(param);
	param.args= sizeof("ef0:/PSP/GAME/FREESTOR/DEPACKAGER/EBOOT.PBP");
	param.argp="ef0:/PSP/GAME/FREESTOR/DEPACKAGER/EBOOT.PBP";
	param.key="game";

	sctrlKernelLoadExecVSHMs1( "ef0:/PSP/GAME/FREESTOR/DEPACKAGER/EBOOT.PBP" ,&param);
	sceKernelExitGame();
	return 0;
}


int exit_callback(int arg1, int arg2, void* common){
	sceKernelExitGame();
	return 0;
}

int CallbackThread(SceSize args, void* argp) {
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

int SetupCallbacks(void) {
	int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}
	return thid;
}


int main() { //In C++ `auto main() -> int` is also valid.
      scePowerSetClockFrequency(333, 333, 166);
	SetupCallbacks();
	pspDebugScreenInit();

      sceCtrlSetSamplingCycle(0);
      sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

      struct SceCtrlData padData;
      struct SceCtrlData ctrlData;
      struct SceCtrlLatch latchData;
      int choosegame = 8;
      int i;
      
      space_free  = psp_get_memory_stick_free();
      //space_free /= 1024 * 1024;
      //space_free = 4096 + space_free;
      sprintf(free_space, "%li", space_free);
      DrawTable();
      drawUI();
      
      //free_space = space_free;
      
      //FILE* file3  = fopen("testing.txt","w");
         //   fputs(free_space , file3 );
           // fclose(file3);


      while (1)
            {
            sceCtrlReadBufferPositive(&ctrlData, 1);
            sceCtrlReadLatch(&latchData);
                  if (latchData.uiMake & PSP_CTRL_DOWN){
                  if (choosegame < 239) {
                  pspDebugScreenPutChar(0,choosegame,0xFFFFFF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x000000, '>');
                  pspDebugScreenPutChar(409,choosegame,0x000000, '>');
                  choosegame = choosegame + 8;
                  pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(409,choosegame,0x0000FF, '>');
                  gamecounter = gamecounter + 1;
                  
                  
                  sprintf(tempstr, "%d", gamecounter);
                  /*std::stringstream ss;
                  ss << gamecounter;
                  std::string tempstr = ss.str();*/
                  pspDebugScreenSetXY(59,33); 
                  pspDebugScreenPrintf("    ");
                  pspDebugScreenSetXY(59,33);
                  pspDebugScreenPrintf(tempstr);
                  
                  } 
		}


                  if (latchData.uiMake & PSP_CTRL_UP){
			if (choosegame > 8) {
                  pspDebugScreenPutChar(0,choosegame,0xFFFFFF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x000000, '>');
                  pspDebugScreenPutChar(409,choosegame,0x000000, '>');
                  
                  choosegame = choosegame - 8;
                  pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(409,choosegame,0x0000FF, '>');
                  gamecounter = gamecounter - 1;
                  sprintf(tempstr, "%d", gamecounter);
                  /*std::stringstream ss;
                  ss << gamecounter;
                  std::string tempstr = ss.str();*/
                  pspDebugScreenSetXY(59,33); 
                  pspDebugScreenPrintf("    ");
                  pspDebugScreenSetXY(59,33);
                  pspDebugScreenPrintf(tempstr);
                  }
		}


                  if (latchData.uiMake & PSP_CTRL_RTRIGGER){
                  if (triangle_pressed == 0 && square_pressed == 0) {
                  if (gamecounter < 1590) {
                  pspDebugScreenInit();
            
                  gamecounter = gamecounter + 30;
                  temp1 = gamecounter% 30;
                  temp1 = gamecounter - temp1 ;
                  sprintf(tempstr, "%d", gamecounter);
                  pspDebugScreenSetXY(59,33); 
                  pspDebugScreenPrintf("    ");
                  pspDebugScreenSetXY(59,33);
                  pspDebugScreenPrintf(tempstr);
                  pspDebugScreenSetXY(0,1);
                  DrawTable();
                  }
                  drawUI();
                  pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(409,choosegame,0x0000FF, '>');
                  }
                  }

            if (latchData.uiMake & PSP_CTRL_LTRIGGER){
            if (triangle_pressed == 0 && square_pressed == 0) {
            if (gamecounter > 30) {
            
            pspDebugScreenInit();
            
            gamecounter = gamecounter - 30;
            temp1 = gamecounter% 30;
            temp1 = gamecounter - temp1 ;
            sprintf(tempstr, "%d", gamecounter);
            pspDebugScreenSetXY(59,33); 
            pspDebugScreenPrintf("    ");
            pspDebugScreenSetXY(59,33);
            pspDebugScreenPrintf(tempstr);
            pspDebugScreenSetXY(0,1);
            DrawTable();
            }
            drawUI();
            pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
            pspDebugScreenPutChar(306,choosegame,0x0000FF, '>');
            pspDebugScreenPutChar(409,choosegame,0x0000FF, '>');
            }
            }


            if (latchData.uiMake & PSP_CTRL_TRIANGLE)   {
            triangle_pressed = 1;
            choosegame = 0;
            gamecounter =0;
            pspDebugScreenInit();
            drawUI();
            pspDebugScreenSetTextColor(0x33FF33);
            pspDebugScreenSetXY(15,27);
            pspDebugScreenPrintf("Press Dpad Up/Dpad Down to select sort");
            pspDebugScreenSetXY(16,28);
            pspDebugScreenPrintf("Press (START) to select sort method");
            pspDebugScreenSetTextColor(0xFFFFFF);
            pspDebugScreenSetXY(3,1);
            pspDebugScreenPrintf("-All games sort by NAME (A..Z)");
            pspDebugScreenSetXY(3,2);
            pspDebugScreenPrintf("-All games sort by SIZE (1..999mb)");
            pspDebugScreenSetXY(3,3);
            pspDebugScreenPrintf("-EU PSP Games sort by NAME (A..Z)");
            pspDebugScreenSetXY(3,4);
            pspDebugScreenPrintf("-EU PSP Games sort by SIZE (1..999mb)");
            pspDebugScreenSetXY(3,5);
            pspDebugScreenPrintf("-US PSP Games sort by NAME (A..Z)");
            pspDebugScreenSetXY(3,6);
            pspDebugScreenPrintf("-US PSP Games sort by SIZE (1..999mb)");
            pspDebugScreenSetXY(3,7);
            pspDebugScreenPrintf("-JP PSP Games sort by NAME (A..Z)");
            pspDebugScreenSetXY(3,8);
            pspDebugScreenPrintf("-JP PSP Games sort by SIZE (1..999mb)");
            pspDebugScreenSetXY(3,9);
            pspDebugScreenPrintf("-Minis only sort by NAME (A..Z)");
            pspDebugScreenSetXY(3,10);
            pspDebugScreenPrintf("-Minis only sort by SIZE (1..999mb)");
            pspDebugScreenSetXY(3,11);
            pspDebugScreenPrintf("-Miscellaneous (PC Engine,NeoGeo)");
            pspDebugScreenSetXY(3,12);
            pspDebugScreenPrintf("-PS1 Games sort by NAME (A..Z)");
            pspDebugScreenSetXY(3,13);
            pspDebugScreenPrintf("-PS1 Games sort by SIZE (1..999mb)");

            }



            if (latchData.uiMake & PSP_CTRL_CROSS)   {
            if (triangle_pressed == 0 && square_pressed == 0) {
            
            temp1 = gamecounter;
            pspDebugScreenInit();
            FILE* file  = fopen(Choose_database,"r");
            memset(extraline, '\0', sizeof(extraline));
            fgets (download_url, 300, file);
            for (int i=0; i < temp1 ;i++) {
            memset(download_url, '\0', sizeof(download_url));
            fgets (download_url, 300, file);}
            //pspDebugScreenPrintf(download_url);
            
            char* pos3 = strstr(download_url,"http");
           char download_urltwo[300];
            strncpy(download_urltwo, download_url, 300);
             
            char* gamesize = strstr(download_urltwo,".pkg	");



            char* pos2 = strstr(pos3,"pkg");  
            memcpy(extraline,pos3,pos2-pos3);
            fclose(file);
           // memset(extraline, '\0', sizeof(extraline));

            FILE* file2  = fopen("url.txt","w");
           



            char* link_end = strstr(extraline,"XXXX");
           
            memset(download_url, '\0', sizeof(download_url));
            memcpy(download_url,extraline,link_end-extraline);

            link_end=link_end+4;
            /*
            pspDebugScreenPrintf(download_url);
            pspDebugScreenPrintf(".dl.p");
            pspDebugScreenPrintf("lays");
            pspDebugScreenPrintf("tati");
            pspDebugScreenPrintf("on.net/cdn/");
            pspDebugScreenPrintf(link_end);
            pspDebugScreenPrintf("pkg");
            */





            fputs(download_url , file2 );
            fputs(".dl.p" , file2 );
            fputs("lays" , file2 );
            fputs("tati" , file2 );
            fputs("on.net/cdn/" , file2 );
            fputs(link_end , file2 );
            fputs("pkg" , file2 );



            fclose(file2);
            gamesize[strlen(gamesize)-2] = '\0';
            gamesize= gamesize+5;
            int gamesizeint = atoi(gamesize);
            //pspDebugScreenPrintf(gamesize);
            //pspDebugScreenPrintf("hello!!!");
            
            gamesizeint = (space_free - (gamesizeint + gamesizeint));
            if (gamesizeint < 0) {
            pspDebugScreenSetTextColor(0x0000CC);
            pspDebugScreenPrintf("\n NOT ENOUGH SPACE ON THE MEMORY CARD! PLEASE FREE (");
            pspDebugScreenPrintf(gamesize);
            pspDebugScreenPrintf(" + ");
            pspDebugScreenPrintf(gamesize);
            pspDebugScreenPrintf(") MB");
            pspDebugScreenSetTextColor(0xFFFFFF);
            pspDebugScreenPrintf("\n \n PRESS START TO CONTINUE");
            triangle_pressed = 1;


            //FILE* file5  = fopen("url.txt","w");
            //fputs(gamesizeint , file5 );
            //fclose(file5);

            }

            if (gamesizeint > 0) {
            //int gamesizeint = atoi(gamesize);
            //if (gamesizeint > 300) {
            //gamesizeint = (gamesizeint * 4.6)
            //}      
            C_downloader(); 
            }
            }
            } 








            if (latchData.uiMake & PSP_CTRL_START)   {
            if (triangle_pressed == 1 || square_pressed == 1) {

            if (gamecounter < 14 && gamecounter > 0) {
            if (gamecounter == 1) Choose_database = "important_files/All_games_sort_by_Name.tsv";
            if (gamecounter == 2) Choose_database = "important_files/All_games_sort_by_Size.tsv";
            if (gamecounter == 3) Choose_database = "important_files/EU_PSP_games_sort_by_Name.tsv";
            if (gamecounter == 4) Choose_database = "important_files/EU_PSP_games_sort_by_Size.tsv";
            if (gamecounter == 5) Choose_database = "important_files/US_PSP_games_sort_by_Name.tsv";
            if (gamecounter == 6) Choose_database = "important_files/US_PSP_games_sort_by_Size.tsv";
            if (gamecounter == 7) Choose_database = "important_files/JP_PSP_games_sort_by_Name.tsv";
            if (gamecounter == 8) Choose_database = "important_files/JP_PSP_games_sort_by_Size.tsv";
            if (gamecounter == 9) Choose_database = "important_files/Minis_only_sort_by_Name.tsv";
            if (gamecounter == 10) Choose_database = "important_files/Minis_only_sort_by_Size.tsv";
            if (gamecounter == 11) Choose_database = "important_files/Mics_PCE_NeoGeo.tsv";
            if (gamecounter == 12) Choose_database = "important_files/PSX_sort_by_Name.tsv";
            if (gamecounter == 13) Choose_database = "important_files/PSX_sort_by_Size.tsv";
            if (gamecounter == 12) ps1_choose = 12;
            if (gamecounter == 13) ps1_choose = 13;
            }
            pspDebugScreenInit();
            triangle_pressed = 0;
            square_pressed = 0;
            choosegame = 0;  
            gamecounter =0;
            temp1 = 0;
            DrawTable();
            drawUI();
            }}


            

            if (latchData.uiMake & PSP_CTRL_SQUARE)   
            {
            square_pressed = 1;
            choosegame = 0;
            gamecounter =0;
            pspDebugScreenInit();
            //drawUI();
            pspDebugScreenSetTextColor(0x33FF33);
            pspDebugScreenSetXY(0,0);
            pspDebugScreenPrintf(" FREESTOR v0.012\n");
            pspDebugScreenPrintf(" made by GorGylka, huge thanks to XProger, RealYoti\n");
            pspDebugScreenSetXY(20,28);
            pspDebugScreenPrintf("Press (START) to main menu");
            pspDebugScreenSetTextColor(0xFFFFFF);
            }






            }
	return 0;
}
