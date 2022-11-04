/*
╔══════════════════════════════════════════════════════════════════════════════════════════════╗
║                        Fonctionnement des librairies partagées d'AmigaOS                     ║
╚══════════════════════════════════════════════════════════════════════════════════════════════╝

Les librairies d'AmigaOS sont des librairies partagées entre différent processus, chargée à
l'exécution d'un programme en ayant besoin.
Il ne faut pas confondre avec les librairies statiques utilisées lors du linking d'un programme C
comme amiga.lib, auto.lib, vc.lib. Le code de ces librairies statiques est insérée par le linkeur
dans notre programme final. Seules les fontions qu'on utilise dans le code C sont insérées dans le
programme. Ces librairies statiques sont habituellement fournies par l'éditeur du compilateur.

Pour utiliser une librairie d'AmigaOS, il faut pendant l'exécution de notre programme, appeler la
fonction OpenLibrary() de la librairie Exec. Et il faudra la fermer avec CloseLibrary()

La librairie à charger peut-être en mémoire ou sur disque, c'est transparent pour l'appelant.
http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_3._guide/node0222.html
Mais pour ouvrir une librairie sur disque, il faut déjà avoir ouvert dos.library.

La librairie Exec.library est toujours en mémoire.
*/

#include <proto/exec.h> /* Exec Library pour OpenLibrary()        */
#include <proto/dos.h>  /* Dos  Library pour Output() et Write()  */

/*
  Chaque librairie a une adresse de base. C'est la valeur retournée par OpenLibrary() et c'est ce
  qui sera stockée dans les variables globales comme DOSBase.
*/
struct DosLibrary *DOSBase;
/*
  Cette adresse de base permet de retrouver toutes les adresses des fonctions de la librairie.
  Cette struct contient aussi des propriétés, cf. fichiers .H

  L'adresse de base de la librairie Exec est toujours stockée à l'adresse mémoire n°4. C'est la
  seule adresse fixe dans AmigaOS. Cela permet de récupérer le pointeur appelé "SysBase" sur la
  struct ExecBase.
  La valeur de la variable dans le monde C "SysBase" est réglée dans les startups codes de VBCC.
 */


int main()
{
    /*
      OpenLibrary() retourne un pointeur sur struct Library. On le transforme en pointeur sur
      struct DosLibrary.
      Le n°36 indique la version minimale requise de la librairie. 36 = KS 2.00

      Pour pouvoir appeler OpenLibrary, il faut que le symbôle "SysBase" soit défini (pointeur de
      base la librairie Exec)
      C'est fait par les startup codes startup.o et minstart.o qui sont linkés sauf si on indique
      l'option -nostdlib au linker.
    */
    int dosVersion = 0;
    DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 36);

    if (DOSBase)
    {
      dosVersion = 36;
    }
    else
    {
      /*
        Je tente avec OldOpenLibrary() qui a l'avantage de fonctionner aussi sous KS 1.0.
        Sous KS 1.0 OpenLibrary() ne marche pas et retourne toujours NULL.
        OldOpenLibrary() ne vérifie pas la version de la librairie.
      */

      DOSBase = (struct DosLibrary *) OldOpenLibrary("dos.library");
    }

    /*
      À ce stade de l'exécution de notre programme, comment retrouver l'adresse mémoire d'une
      fonction d'une librairie AmigaOS ?

      Sachant que la librairie peut être chargée à des endroits différents en mémoire à chaque
      exécution, le linker n'a aucun moyen de lier notre appel dans le code source C avec la
      position de la fonction AmigaOS en mémoire lors de l'exécution de notre programme.
      (C++ règle ce problème de son côté avec les tables de méthodes virtuelles)

      En fait, chaque fonction fournie par la librairie AmigaOS a une constante associée avec elle.
      Cette constante s'appelle son Library Vector Offset ou LVO.
      Lors de l'exécution de notre programme, en retrachant cette constante à l'adresse de base de
      la librairie, on récupére l'adresse de la routine.
      C'est fait par :
      - des macros du préprocesseur C qui génère des appels inline. C'est le mode d'appel par défaut
        avec VBCC (et d'autres compilateurs Amiga)
      - ou bien par des fonctions "stubs" dans la librairie Amiga.lib fournie avec VBCC, avec
        laquelle on linke dans ce cas le programme.

      • Pour les appels inline :
      En regardant, les fichiers include/inline/*_protos.h, on peut voir que le nom DOSBase est
      utilisé dans des macros.
      Ces macros transforment les appels qu'on fait dans notre code C en appel inline à la fonction
      réelle de la librairie AmigaOS.

      Exemple de la fonction Write() dans targets\m68k-amigaos\include\inline\dos_protos.h :

      LONG __Write(__reg("a6") void *, __reg("d1") BPTR file, __reg("d2") CONST_…)="\tjsr\t-48(a6)";
      #define Write(file, buffer, length) __Write(DOSBase, (file), (buffer), (length))

      La macro #define Write(...) ajoute le nom DOSBase en appellant la fonction __Write() dont le
      court code assembleur est indiqué :
      jsr -48(a6)       → appelle la routine à l'adresse contenue dans le registre a6 - 48.
      48 est le LVO de la fonction Write() dans la dos.library
      Le prototype de la fonction __Write indique aussi dans quels registres du CPU passer les
      paramètres, dans ce cas d1, d2, d3, cf. doc de la fonction Write()

      La fonction main est alors compilée comme ça par VBCC en asm (option -k pour conserver le .ASM
      intermédiaire) :
        (...)
      	moveq	  #14,d3
        move.l	#l6,d2
        move.l	d4,d1
        move.l	_DOSBase,a6
        jsr	    -48(a6)               ; Jump To Subroutine
        (...)

        Note : l6 c'est la chaine "Hello, world!\n" et 14 sa longueur.


      • Pour les appels non inline

       Le code assembleur de la fonction main() :
        (...)
        move.l	#14,-(a7)             ; met le paramètre "length" dans la pile (Le registre a7 est
                                      ; le pointeur de la pile en 68000)
        pea   	l6
        move.l	d2,-(a7)
        jsr	    _Write                ; Jump To Subroutine → fonction _Write de Amiga.lib
        (...)

        La fonction _Write de Amiga.lib met les paramètres dans les registres adéquats
        et appelera réllement la fonction de la librairie AmigaOS. J'ai pu voire le code de la
        fonction stub _Write de Amiga.lib désassemblée en débuguant sous VSCode avec
        amiga-assembly-extension

          movem.l d2-d3, -(a7)        ; ← 1ère instruction de la fonction _Write() de Amiga.lib
                                      ; qui sauvegarde les registre d2-d3 dans la pile
          movea.l $20d578.l, a6       ; met l'adresse DOSBase dans le registre a6
          movem.l $10(a7), d1-d3      ; met les paramètres attendu par Write() de Dos.library
                                      ; dans les registres attendus par Write() : d1, d2 et d3
          jsr -48(a6)                 ; Appel dans Dos.library de Write(), reconnaissable au LVO -48

          movem.l (a7)+, d2-d3/a6     ; Après l'appel à Write(), restaure les valeurs des registres

          rts                         ; Return from Subroutine
    */


    if (DOSBase)
    {
        /* Output() retourne un descripteur de fichier sur ce qui est appelé "stdout" en C.
           Il faut l'appeler différemment pour éviter les doublons si on inclut stdio.h */
        BPTR out = Output();

        const char hello[] = "Hello, world!\n";

        Write(out, hello, sizeof(hello));
        /* Fichier, Ptr sur données, Longueur

           sizeof() sur une chaine constante permet une évaluation dès la compilation de la longueur
        */

        if (dosVersion >= 36)
        {
          /*
            VPrintf est similaire printf() de la librairie standard C, mais elle disponible dans
            dos.library à partir de V36=Kickstart 2.0.
            Elle a besoin d'un pointeur sur un tableau argv pour les arguments qui vont remplir la
            chaîne de caractères.
            VPrintf déréférence le pointeur. Cela rend compliqué son utilisation quand on est
            habitué à printf() du C
            http://amigadev.elowar.com/read/ADCD_2.1/Includes_and_Autodocs_3._guide/node004E.html
          */
          VPrintf("VPrintf SysBase->LibNode.lib_IdString=%s", & (SysBase->LibNode.lib_IdString));
        }

        CloseLibrary((struct Library *)DOSBase);
    }

    return 0;
}
