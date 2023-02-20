#include "callbacks.hpp"

#include <pspkernel.h>
#include <iostream>
#include <psputility.h>

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
  (void)arg1;
  (void)arg2;
  (void)common;
  std::cout << "Exiting..." << std::endl;
  sceUtilityNetconfShutdownStart();
  sceKernelExitGame();
  return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
  (void)args;
  (void)argp;

  int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
  sceKernelRegisterExitCallback(cbid);

  sceKernelSleepThreadCB();

  return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
  int thid = 0;

  thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
  if (thid >= 0)
    sceKernelStartThread(thid, 0, 0);

  return thid;
}
