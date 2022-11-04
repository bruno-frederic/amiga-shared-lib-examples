/****************************************

 PT-Player 2.0   © 1994 BetaSoft
 
 uses PTReplay.library also by BetaSoft
 and Andreas [Pucko] Pålsson
 
****************************************/

#include <exec/types.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>
#include <dos/dos.h>
#include <stdio.h>
#include <stdlib.h>

#include "/include/ptreplay.h"
#include "/include/ptreplay_protos.h"
#include "/include/ptreplay_pragmas.h"

struct Library *PTReplayBase;
struct Module *Mod = NULL;

char *vstr="$VER: SmallPlay 2.0 (23.12.93)";

int main(int argc, char **argv)
{
BYTE SigBit;
ULONG SigMask;

	if(argc > 2)
		{ puts("specify one module only\n"); return 20; }
	if(argc < 2)
		{ puts("play what module?\n"); return 20; }

	if(PTReplayBase = OpenLibrary("ptreplay.library",0L))
	{
		if(Mod = PTLoadModule(argv[1]))
		{
			if((SigBit=AllocSignal(-1))==-1)
			{
				puts("couldn't allocate signal\n");
				exit(0);
			};

			PTInstallBits(Mod, SigBit, -1, -1, -1);

			PTPlay(Mod);

			SigMask=Wait(SIGBREAKF_CTRL_C | 1L<<SigBit);

			if(SigMask & SIGBREAKF_CTRL_C)
				PTFade(Mod,1);
			else
				PTStop(Mod);

			FreeSignal(SigBit);

			PTUnloadModule(Mod);

		}else puts("couldn't open/load module\n");
		CloseLibrary(PTReplayBase);

	} else puts("couldn't open ptreplay.library\n");

	return(0);
}
