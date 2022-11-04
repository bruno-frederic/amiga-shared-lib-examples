/****************************************

 PT-Player 2.0   © 1994 BetaSoft

 uses PTReplay.library also by BetaSoft
 and Andreas [Pucko] Pålsson

****************************************/

//#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdio.h>
#include <stdlib.h>

#include <proto/ptreplay.h>

struct Library *PTReplayBase;
struct Module *Mod = NULL;

char *vstr="$VER: SmallPlay 2.0 (23.12.93)";

int main(int argc, char **argv)
{
	puts("begin main(...)\n");

	if(argc > 2)
		{ puts("specify one module only\n"); 			return RETURN_ERROR; }
	if(argc < 2)
		{ puts("play what module?\n"); 					return RETURN_ERROR; }

	puts("OpenLibrary(\"ptreplay.library\",0L)\n");
	PTReplayBase = OpenLibrary("ptreplay.library",0L);
	if (! PTReplayBase)
		{ puts("couldn't open ptreplay.library\n"); 	return RETURN_FAIL; }

	puts("AllocSignal(-1)\n");
	// http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_2._guide/node0333.html
	BYTE SigBit=AllocSignal(-1);
	if (SigBit == -1)
		{ puts("couldn't allocate signal\n");			return RETURN_FAIL; }
	printf("SigBit = %i\t", SigBit);
	ULONG PTSignal = 1UL << SigBit;
	printf("PTSignal = %lu\n", PTSignal);

	printf("PTLoadModule(%s)\n", argv[1]);
	Mod = PTLoadModule(argv[1]);
	if (! Mod)
		{ printf("couldn't open/load module: %s\n", argv[1]);			return RETURN_ERROR; }
	/*
		Attention : SmallPlay ne lit pas vraiment le module quand je ne configure pas correctement
		le "debugging_trigger=(...)" dans le launch.json de VS Code ! Je ne comprends pas pourquoi.
		Je suppose que la version customisé de WinUAE pour le debugging vient perturber.
	*/

	puts("PTInstallBits(...)\n");
	// This will set signal SigBit when the module restarts.
	PTInstallBits(Mod, SigBit, -1, -1, -1);

	printf("PTPlay(%s)\n", argv[1]);
	PTPlay(Mod);

	puts("Wait(SIGBREAKF_CTRL_C | PTSignal)\n");
	ULONG signals = Wait(SIGBREAKF_CTRL_C | PTSignal);
	printf("End Wait() -> signals=%lu\t", signals);

	if(signals & SIGBREAKF_CTRL_C)
	{
		puts("SIGBREAKF_CTRL_C\n");
	}

	if(signals & PTSignal)
	{
		puts("PTSignal\n");
	}

	PTStop(Mod);

	FreeSignal(SigBit);

	PTUnloadModule(Mod);


	puts("CloseLibrary(PTReplayBase);\n");
	CloseLibrary(PTReplayBase);



	return RETURN_OK;
}
