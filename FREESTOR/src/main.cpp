/**
 * @file main.cpp
 * @author KorigamiK
 * @brief A cURL test for the PSP which allows a Net Dialog to connect to a
 * netowrk and then downloads a file from the internet
 * @version 0.1
 * @date 2023-01-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <pspdebug.h>
#include <pspkernel.h>
#include <pspctrl.h>

#include <iostream>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "callbacks.hpp"
#include "file_handler.hpp"
#include "graphics.hpp"
#include "network.hpp"

#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define URL_FIlE "./url.txt"

PSP_MODULE_INFO("cURL Test", 0, 1, 1);



int temp1 = 0;
int triangle_pressed = 0;


const char* Choose_database = "important_files/All_games_sort_by_Name.tsv";





void drawUI()
      {
      pspDebugScreenSetTextColor(0x00DDDD);
      pspDebugScreenSetXY(0,31);
      pspDebugScreenPrintf("____________________________________________________________________");
      pspDebugScreenSetXY(0,32);
      pspDebugScreenPrintf("UP,DOWN=select game  X=install game TRNGL=sort games");
      pspDebugScreenSetXY(0,33);
      pspDebugScreenPrintf("L,R=page next/previous []=credits"); 
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


void drawTable () {
      std::string str;
      std::ifstream file2(Choose_database);
      int xx = 1;
      std::getline(file2,str);
      for (int i=0; i < temp1 ;i++) {
      file2.ignore(9999,'\n');                  
      //std::getline(file2,str);
      }

      pspDebugScreenSetXY(0,1);
      for (int i=0; i < 30 ;i++)
            {
            std::getline(file2,str);
            
            std::string str3 = str.substr(0, str.find("	"));
            pspDebugScreenSetXY(2,xx);
            int ab = 0;
            ab = int(str3.length());
            if (ab > 37 ) {
            str3.erase(37);
            str3.append("...");    
            }
            pspDebugScreenPrintf(str3.c_str()); //patapon for example
            
            std::size_t pos = str.find("	");
            std::string str2 = str.substr (pos);
            str2.erase (0,1);

            str3 = str2.substr(0, str2.find("	"));
            pspDebugScreenSetXY(45,xx);
            pspDebugScreenPrintf(str3.c_str()); //minis for example

            std::size_t pos1 = str2.find("	");
            str3 = str2.substr (pos1);
            str3.erase (0,1);

            str2 = str3.substr(0, str3.find("	"));
            pspDebugScreenSetXY(57,xx);
            pspDebugScreenPrintf(str2.c_str()); //EU for example

            std::size_t pos2 = str3.find("	");
            str2 = str3.substr (pos2);
            str2.erase (0,1);

            std::size_t pos3 = str2.find("	"); //skip http link coz i dont need it
            str3 = str2.substr (pos3);
            str3.erase (0,1);
            str3.pop_back();
            str3.append(" MB");  
            pspDebugScreenSetXY(61,xx);
            pspDebugScreenPrintf(str3.c_str()); //300 MB for example
            
            
            
            xx = (xx + 1);
            }
      xx = 1;
      file2.close();

}






























int main_thread(SceSize args, void *argp)
{
	(void)args;
	(void)argp;

	// Get the URL from the file
	std::string url = get_file_contents(URL_FIlE);

	pspDebugScreenPrintf("\n%s\n", url.c_str());

	// Download the file
	pspDebugScreenPrintf("\nDownloading...\n");
	curlDownload(url) < 0 ? pspDebugScreenPrintf("Failed!\n") : pspDebugScreenPrintf("Success!\n");

	return 0;
}

void init()
{
	setupGu();

	pspDebugScreenInit();
	pspDebugScreenSetXY(0, 0);

	pspDebugScreenSetBackColor(0x000000);
	pspDebugScreenSetTextColor(0xFFFFFF);
	pspDebugScreenClear();

	startNetworking();
}

void close()
{
	stopNetworking();
}

void curl_main()
{
	//(void)argc;
	//(void)argv;

	init();
	//pspDebugScreenPrintf("cURL Test");

	main_thread(0, NULL);

	close();

	printf("Press X to exit\n");
	pspDebugScreenPrintf("Press X to exit");
	while (1)
	{
		SceCtrlData pad;
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_CROSS)
			break;
	}

	sceKernelExitGame();


}





















auto main() -> int
{
      SetupCallbacks();
      pspDebugScreenInit();
	

      sceCtrlSetSamplingCycle(0);
      sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

      struct SceCtrlData padData;
      struct SceCtrlData ctrlData;
      struct SceCtrlLatch latchData;
      
      
      const int number_of_games = 2243;
      int gamecounter = 0;
      std::string tempstr;
      int i;
      std::string str;
      
 
      

      //pspDebugScreenInit();
      /*int choosegame = 8;
      while (choosegame < 247) {
      pspDebugScreenPutChar(0,choosegame,0xFFFFFF, '>');
      choosegame = choosegame + 8;
      }
      */
      int choosegame = 0;
      //pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
      drawTable();
      drawUI();

      while (true)
      {     sceCtrlReadBufferPositive(&ctrlData, 1);
            sceCtrlReadLatch(&latchData);
            








            if (latchData.uiMake & PSP_CTRL_DOWN){
                  if (choosegame < 239) {
                  pspDebugScreenPutChar(0,choosegame,0xFFFFFF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x000000, '>');
                  pspDebugScreenPutChar(417,choosegame,0x000000, '>');
                  choosegame = choosegame + 8;
                  pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(417,choosegame,0x0000FF, '>');
                  gamecounter = gamecounter + 1;
                  

                  std::stringstream ss;
                  ss << gamecounter;
                  std::string tempstr = ss.str();
                  pspDebugScreenSetXY(60,33); 
                  pspDebugScreenPrintf("    ");
                  pspDebugScreenSetXY(60,33);
                  pspDebugScreenPrintf(tempstr.c_str());
                  
                  } 
		}








            if (latchData.uiMake & PSP_CTRL_UP){
			if (choosegame > 8) {
                  pspDebugScreenPutChar(0,choosegame,0xFFFFFF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x000000, '>');
                  pspDebugScreenPutChar(417,choosegame,0x000000, '>');
                  choosegame = choosegame - 8;
                  pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(306,choosegame,0x0000FF, '>');
                  pspDebugScreenPutChar(417,choosegame,0x0000FF, '>');
                  gamecounter = gamecounter - 1;
                  std::stringstream ss;
                  ss << gamecounter;
                  std::string tempstr = ss.str();
                  pspDebugScreenSetXY(60,33); 
                  pspDebugScreenPrintf("    ");
                  pspDebugScreenSetXY(60,33);
                  pspDebugScreenPrintf(tempstr.c_str());
                  }
		}
            
            
            
            
            
            
            
            
            
            
            if (latchData.uiMake & PSP_CTRL_RTRIGGER){
            if (triangle_pressed == 0) {
            if (gamecounter < 1590) {
            pspDebugScreenInit();
            
            gamecounter = gamecounter + 30;
            temp1 = gamecounter% 30;
            temp1 = gamecounter - temp1 ;
            std::stringstream ss;
            ss << gamecounter;
            std::string tempstr = ss.str();
            pspDebugScreenSetXY(60,33); 
            pspDebugScreenPrintf("    ");
            pspDebugScreenSetXY(60,33);
            pspDebugScreenPrintf(tempstr.c_str());
            pspDebugScreenSetXY(0,1);
            drawTable();
            }
            drawUI();
            pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
            pspDebugScreenPutChar(306,choosegame,0x0000FF, '>');
            pspDebugScreenPutChar(417,choosegame,0x0000FF, '>');
            }
            }
            
            
            
            
            
            
            
            
            
            if (latchData.uiMake & PSP_CTRL_LTRIGGER){
            if (triangle_pressed == 0) {
            if (gamecounter > 30) {
            
            pspDebugScreenInit();
            
            gamecounter = gamecounter - 30;
            temp1 = gamecounter% 30;
            temp1 = gamecounter - temp1 ;
            std::stringstream ss;
            ss << gamecounter;
            std::string tempstr = ss.str();
            pspDebugScreenSetXY(60,33); 
            pspDebugScreenPrintf("    ");
            pspDebugScreenSetXY(60,33);
            pspDebugScreenPrintf(tempstr.c_str());
            pspDebugScreenSetXY(0,1);
            drawTable();
            }
            drawUI();
            pspDebugScreenPutChar(0,choosegame,0x0000FF, '>');
            pspDebugScreenPutChar(306,choosegame,0x0000FF, '>');
            pspDebugScreenPutChar(417,choosegame,0x0000FF, '>');
            }
            }









            if (latchData.uiMake & PSP_CTRL_CROSS)   {
            if (triangle_pressed == 0) {
            temp1 = gamecounter;
            pspDebugScreenInit();
            std::ifstream file2(Choose_database);
            std::getline(file2,str);
            for (int i=0; i < gamecounter ;i++) {
            std::getline(file2,str);}
            pspDebugScreenPrintf(str.c_str());
            str = str.substr(0, str.find(".pkg"));
            //str2.erase (0,15);
            std::size_t pos4 = str.find("http://");
            str = str.substr (pos4);

            file2.close();
            //std::ofstream file("test.txt");
            std::ofstream file("ms0:/PSP/GAME/FREESTOR/url.txt");
            file << (str);
            file << (".pkg");
            file.close();
            curl_main();
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

            }



            if (latchData.uiMake & PSP_CTRL_START)   {
            if (triangle_pressed == 1) {

            if (gamecounter < 12 && gamecounter > 0) {
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
                  pspDebugScreenInit();
                  triangle_pressed = 0;
                  choosegame = 0;
                  gamecounter =0;
                  temp1 = 0;
                  drawTable();
                  drawUI();
                  
                  }
            }



            }


      }
      
      return 0;
}


