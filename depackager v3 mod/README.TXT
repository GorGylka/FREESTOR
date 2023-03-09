This is a homebrew to install PSN PSP pkgs on the PSP.
pkgs should be placed at ms0:/pkg/, if that folder does not exist, then create it.

place included homebrew on your PSP.
	ms0:/PSP/GAME/PSP depackager/EBOOT.PBP

This homebrew only supports installing PSP retail pkgs and files, PS3/debug pkgs are not supported.

boring info, v1 and v2 uses the MagicGate hardware for the AES crypto,
no idea about any benefits/disadvantages with it compared to the kirk engine.

Thanks to @ReRepRep for the idea.

Changelog:
v3:
	Now uses software AES crypto (not kirk), so it should be ePSP compatible too.
v2:
	Added KEYS.BIN extraction for PS1 pkgs.
v1:
	Initial release.