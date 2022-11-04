# PTReplay library and SmallPlay Example

For explanations, look at the comments of the .c files and *Makefile*.

## Additionnal Requirement for PTReplay library and SmallPlay example
- AmigaOS 1.3 NDK header files (from Amiga Developer CD v2.1) installed in `${VBCC}/NDK_1.3/INCLUDES1.3/INCLUDE.H`
- fd2pragma built from [Frank Wille sources](http://sun.hasenbraten.de/~frank/projects/)


## Compatibility

 J'ai fait plein de tests de lancement de l'exécutable généré et je constate des problèmes de fiabilité  en utilisant la librairie PTReplay avec le WinUAE 4.9.0 (2021.11.27) 64 bits forké par *prb28* pour le debug, avec ces versions du KickStart (mais OK dans WinUAE 4.4 de Toni Wilen) :
- 1.1 (quickstart=a1000,0)
- 1.2 (quickstart=a500,5)
- 1.3 (quickstart=a500,1)
- 3.0 (quickstart=a4000,0)

 SmallPlay s'exécute mais aucun son ne sort dans WinUAE 4.9 de prb28 ou dans WinUAE 4.4 de Toni :
- 2.0 et 2.02 sur A3000 (quickstart=a3000,0)

 Marche bien dans :
- 2.04  (quickstart=a500+,0)  (1 fois pas de son sur PC mais plein de fois test OK)
- 2.05  (quickstart=a600,0)	  (testé plein de fois de suite sur PC OK)
- 3.0   (quickstart=a4000,0)
- 3.1   (quickstart=a3000,2)
- 3.1   (quickstart=a4000,0)
- 3.1   (quickstart=a1200,0)  (testé plein de fois de suite, mais plante rarement)
- 3.1   (quickstart=a500,1)   (testé plein de fois de suite, mais plante rarement)
- 3.1.4 (quickstart=a500,1)   (testé OK 3x de suite sur 2 PC différents)
- 3.2   (quickstart=a500,1)   (testé OK 3x de suite sur 2 PC différents)
- 3.2.1 (quickstart=a500,1)   (testé OK 3x de suite sur 2 PC différents)

 Enfin ne peut pas marcher sous Kick 1.0 car les EXE produits par VBCC avec son startup code standard (nécessaire à la librairie C standard requise par SmallPlay) ne fonctionnent pas dessus.
- 1.0 (quickstart=a1000,0) Guru Meditation directement, y compris sous WinUAE 4.4 de Toni

 Note : Le dernier numéro en parenthèse indique la position dans la liste déroulante
 "Configuration" de l'écran QuickStart de WinUAE. Une ROM correpondante à la verison du KS doit éventuellement être spécifiée, en plus de la configuration quickstart.
