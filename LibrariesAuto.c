/*
╔══════════════════════════════════════════════════════════════════════════════════════════════╗
║         Auto.lib de VBCC pour nous aider à charger les librairies partagées d'AmigaOS        ║
╚══════════════════════════════════════════════════════════════════════════════════════════════╝

La librairie statique auto.lib fournie avec VBCC simplifie l'ouverture des librairies partagées
d'AmigaOS.

Mais en cas de problème pour ouvrir une librairie, l'utilisateur n'est pas averti de la nature de
l'erreur et la fonction main() n'est même pas démarrée. Seul message d'AmigaOS :
failed returncode 20

L'auteur de VBCC (phx) explique l'implementation faite. Le linkeur VLink génère automatiquement une
liste des "constructeur" des librairies à charger et une liste correspondante de "déstructeur" pour
les décharger après l'exécution de main() :
http://eab.abime.net/showthread.php?p=1172430#post1172430


*/

#include <proto/dos.h>          /* Dos Library pour Output() et Write()     */
#include <proto/graphics.h>     /* Pour VBeamPos()                          */

/*
Il est conseillé de spécifier un numéro de version minimale des lib via les variables spéciale,
cf VBCC.pdf §14.5.9 auto.lib
*/
int _GfxBaseVer = 36; /* version 36 correspond à AmigaOS 2.0, ça plantera avec AmigaOS 1.x */

int main()
{
    BPTR out = Output();

    Write(out, "Hello, from Library Auto!\nCalling :\n", 36);

    /* Avec cet appel à VBeamPos, le compilateur C va générer cette fonction main() en ASM avec
       l'appel inline de VBeamPos :
        (...)
        move.l	_GfxBase,a6
        jsr	-384(a6)
	    move.l	d0,d5
        (...)

        Si on ne linke pas avec -lauto alors le linkeur ne trouve pas le symbole _GfxBase.
        En linkant avec -lauto, alors la library Base va être prise dans auto.lib ainsi que le code
        pour open et close la librairie.
    */
    long l = VBeamPos();                    /* Retourne la position du spot vidéo */
    VPrintf("VBeamPos() -> %ld\n", &l);

    l = VBeamPos();
    VPrintf("VBeamPos() -> %ld\n", &l);

    return 0;
}
