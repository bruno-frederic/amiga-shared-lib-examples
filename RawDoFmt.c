/*
╔══════════════════════════════════════════════════════════════════════════════════════════════╗
║         Exemple of RawDoFmt() function from Exec.library and Write() from DOSLibrary         ║
╚══════════════════════════════════════════════════════════════════════════════════════════════╝

  They are supported on every Kickstart version from 1.0

  http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_3._guide/node0227.html

  J'ai aussi tenté la fonction Printf() mais échoue en KS 1.x :
  http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_3._guide/node0048.html

  Attention : il faut mettre %ld pour les int. %d ne marche que pour les word
              et %u n'apprait qu'en KS 2.04
*/

#include <proto/exec.h>       /* Exec Library pour OpenLibrary()        */
#include <proto/dos.h>        /* Dos  Library pour Output() et Write()  */
#include <string.h>           /* strlen()                               */

struct DosLibrary *DOSBase;

/*
  RawDoFmt a besoin d'une procédure qui doit recopier chaque caractère : PutChProc

  Cette implémentation ASM apparait dans l'AutoDoc 3 de RawDoFmt() et dans cet exemple :
  http://eab.abime.net/showthread.php?p=1318994#post1318994

  J'en ai vu une autre ici :
  https://eab.abime.net/showthread.php?p=1177896#post1177896
      -> PutChProc = 0x52934E75;   // 68000 opcodes for addq.l #1,(A3) then rts

  Ca pourrait se faire avec les instructions ASM de VBCC probablement :
  Piste https://eab.abime.net/showthread.php?t=83618
*/
static const ULONG PutChProc=0x16c04e75; /* move.b d0,(a3)+ ; rts */


int main()
{
  /* OldOpenLibrary() to be KS 1.0 compatible */
  DOSBase = (struct DosLibrary *) OldOpenLibrary("dos.library");
  if (! DOSBase) return RETURN_FAIL;

  BPTR out = Output();
  Write(out, __FILE__, sizeof(__FILE__)); Write(out, ":\n", sizeof(":\n"));

  unsigned char buffer[512];

  // RawDoFmt prend un tableau (donc un pointeur) sur les valeurs à insérer :
  int arr[3];
  arr[0] = 0x00777777;
  arr[1] = 0x00444444;
  arr[2] = 0x00333333;

  /* RawDoFmt est compliquée à utilisée car :
   • si le nombre d'élément dans le tableau n'est pas le même que dans la format string → n'importe
     quoi est inclus !
   • si le type n'est pas bon dans la format string, tout est décalé dans la sortie !
     → Attention donc à bien mettre %ld ; %lx pour les int
  */
  RawDoFmt("format string : 1=0x%08.lx 2=0x%08.lx 3=0x%08.lx\n", arr,
            (void (*)())&PutChProc, buffer);
  Write(out, buffer, strlen(buffer));

  // avec des WORD :
  WORD wordArr[] = { 0x7777, 0x4444, 0x3333 }; // Tableau directement initialisé
  RawDoFmt("format string : 1=0x%x 2=0x%x 3=0x%x\n", wordArr,
           (void (*)())&PutChProc, buffer);
  Write(out, buffer, strlen(buffer));

  // Mais on peut aussi l'appeler avec une seule valeur, c'est plus simple, il suffit de lui
  // passer son adresse/pointeur avec l'opérateur "&":
  int val = 0x00999999;
  RawDoFmt("format string : 1=0x%08.lx\n", &val,
            (void (*)())&PutChProc, buffer);
  Write(out, buffer, strlen(buffer));


  /*
    Note : Cela ne peut pas marcher avec une constante "&1234" écrite directement, comme ceci,
    RawDoFmt veut un pointeur :
  RawDoFmt("format string : %ld\n", &1234,
           (void (*)())&PutChProc, buffer);
  */


  // On limite à 30 caractères le nombre pour se protéger d'un buffer overflow :
  const char msg[] = "SysBase->LibNode.lib_IdString=%.30s";
  unsigned char outBuffer[sizeof(msg) + 30];
  RawDoFmt(msg, &(SysBase->LibNode.lib_IdString),
           (void (*)())&PutChProc, outBuffer);
  Write(out, outBuffer, strlen(outBuffer));

  /* Display Exec version, revision and kickstart revision :

    SysBase->SoftVer is kickstart revision (!= Exec library revision) :
        0 for KickStart  1.x
       63 for Kickstart v3.1  r40.63  (1993)
      175 for Kickstart v2.04 r37.175 (1991)
  */
  UWORD argarray[] = { SysBase->LibNode.lib_Version,
                       SysBase->LibNode.lib_Revision,
                       SysBase->SoftVer };

  const char msg2[] = "lib_Version=%.10d lib_Revision=%.10d SysBase->SoftVer=%.10d\n";
  unsigned char outBuffer2[sizeof(msg2) + 30];
  RawDoFmt(msg2, argarray,
           (void (*)())&PutChProc, outBuffer2);
  Write(out, outBuffer2, strlen(outBuffer2));


  if (DOSBase) CloseLibrary((struct Library *)DOSBase);
  return RETURN_OK;
}
