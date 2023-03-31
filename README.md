# FREESTOR 

![img_version]   ![img_downloads]   ![img_stars]


![img_photo]



This app allows download and install PSP .pkg directly via WiFi, without PC. 

# To install FREESTOR:
___________________________
-copy all files from [RELEASES](https://github.com/GorGylka/FREESTOR/releases) to root of your memory card ms0:/

-install Npdrm_free plugin on your PSP (https://github.com/lusid1/npdrm_free_mod)

<details>
  <summary>HOW TO INSTALL npdrm_free_mod Plugin (click on me)</summary>
  
  -If you have PSP 1000/2000/3000
  
     -Place npdrm_free_mod.prx in ms0:/seplugins folder
  
     -Add line (without quotes) "ms0:/seplugins/npdrm_free_mod.prx 1" to ms0:/seplugins/vsh.txt AND ms0:/seplugins/vsh.txt
  
     -Reboot your PSP
  
  -If you have PSP Go without memory card use this plugin instead ----> [npdrm_free](https://github.com/qwikrazor87/npdrm_free)
  
     -Place npdrm_free.prx in ef0:/seplugins folder
  
     -Add line (without quotes) "ef0:/seplugins/npdrm_free.prx 1" to ef0:/seplugins/vsh.txt AND ef0:/seplugins/vsh.txt
  
     -Reboot your PSP

 -Only for PSP go owners, to make PS1 games work (thanks to RazorStrike)
 
      -Download Popsloader (Google it)
      
      -The zip has SEPLUGINS folder, with POPS.txt and POPSLOADER inside. Copy POPSLOADER folder into ef0:/seplugins/
      
      -Edit the ef0:/seplugins/POPS.txt, add lines 
      
      ef0:/seplugins/popsloader/popsloader.prx 1
      ef0:/seplugins/popsloader/cdda_enabler.prx 1
      
      -Restart the console
      
      -Start the PS1 game holding R. It should open a menu with all Popsloader firmwares, 
      you can choose 1 and it will apply and start the game.
      After that you won't need to do that again, unless the game isn't compatible with the selected firmware,
      but can be changes the same way by holding R when starting the game.
      
      

  
</details>

-to prevent shutdown during download process, disable Auto Sleep (PSP Settings -> Power Save Settings -> Auto Sleep -> off)

- (Optional) Set 333/166 clock speed, if you want to speed up unpacking process 
___________________________
# How to use:

Run FREESTOR, choose game, then press X to download and install pkg. That's it.
___________________________
# Limitations:

-PSP Wifi Does not support new Encryption Standarts, so to use it, change Wifi encryption from WPA2-PSK to WPA-PSK , WEP or just disable password in your wifi router settings

-Maximum download speed is about ̶2̶0̶0̶-̶3̶0̶0̶k̶b̶/̶s̶  450-600kb/s due to outdated Wifi module. Downloading and installing a 500MB game takes approximately 35 minutes

-PSP Street not supported at all

___________________________
# TO DO List:

-w̶r̶a̶p̶ ̶t̶w̶o̶ ̶a̶p̶p̶s̶ ̶i̶n̶t̶o̶ ̶o̶n̶e̶  (Done)

-i̶n̶c̶r̶e̶a̶s̶e̶ ̶d̶o̶w̶n̶l̶o̶a̶d̶ ̶s̶p̶e̶e̶d̶ ̶f̶r̶o̶m̶ ̶2̶5̶0̶ ̶t̶o̶ ̶5̶0̶0̶ ̶k̶b̶/̶s̶ (Done)

-a̶d̶d̶ ̶p̶s̶1̶ ̶l̶i̶b̶r̶a̶r̶y̶  (Done)

-Replace one of the system apps (such as Digital Comics) with a Freeshop app
___________________________
Tested on PSP 3000 6.61 PROMOD-C2p∞ , PSP 2000 6.61 ARK-4

# Support the project with a donation 🤏

USDT  0xbcb360031be3fc6a134b95d1bf20ede5da801921 


[img_downloads]: https://img.shields.io/github/downloads/GorGylka/FREESTOR/total.svg?color=red&style=for-the-badge&maxAge=3600
[img_stars]: https://img.shields.io/github/stars/gorgylka/freestor?color=red&style=for-the-badge&maxAge=3600
[img_version]: https://img.shields.io/github/v/release/gorgylka/freestor?color=red&label=latest%20release&style=for-the-badge
[img_photo]: https://github.com/GorGylka/FREESTOR/blob/main/photo.jpg
