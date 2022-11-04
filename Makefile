CC=vc
ODIR=build-vbcc

ADF=$(subst /,$(PATHSEP),uae/$(notdir $(CURDIR)).adf)   # <=> basename of current folder
UAE_CACHE_FILE=bin/configuration.cache bin/winuaebootlog.txt bin/default.uss bin/winuae_*.dmp
MAKEFILE_UPTODATE=$(ODIR)/Makefile.uptodate

# Prepare variables for target 'clean'
ifeq ($(OS),Windows_NT)
	RM:=del /F /Q
	TOUCH:=COPY /Y NUL
	PATHSEP:=\\
	CONFIG:=${CONFIG}_win
else
	RM:=rm -f
	TOUCH:=touch
	PATHSEP:=/
endif

all: $(MAKEFILE_UPTODATE) $(ADF)


uae/dh0/Libraries : Libraries.c
	@echo =================== COMPIL INLINE+LINKING ===================
#	Pas besoin de la librairie -lamiga : par défaut les fichiers include de la target utilise des
#	appels Inline aux fonctions de la dos.library. Ce sont des macros contenues dans les fichiers .H
#	pour le pré-processeur.
# 	Néanmoins nécessaire d'inclure les .H du NDK sinon : file 'exec\types.h' not found.
#	En effet, les fichiers headers fournis avec les targets VBCC ne remplace pas tout le NDK Amiga,
#	mais uniquement le dossier proto pour pouvoir utiliser le code assembleur spécifique à VBCC pour
#	faire les appels inline.
	$(CC) +aos68km -g -v $< -o $@ \
	      -I%VBCC%/targets/m68k-amigaos/include -I%VBCC%/NDK3.2R4/Include_H
#	aos68km = minimal startup pour gérer nous même l'ouverture de toutes les librairies AmigaOS, y
# 			  compris DOSLibrary

#   Si on défini _NO_INLINE afin d'utiliser les fonctions stub (intermédiaire) dans la librairie
#   amiga.lib pour appeler les fonctions des librairies AmigaOS.
#   et qu'on a des erreurs à la compilation unknown identifier <SysBase>, il faut bien spécifier à
#   VC le dossiers d'includes de la target puis du NDK dans cet ordre :
#   	-I%VBCC%/targets/m68k-amigaos/include -I%VBCC%/NDK3.2R4/Include_H
#   pour atteindre le bon fichier exec.h fourni avec VBCC qui déclare correctement SysBase dans le
#   fichier .H même si _NO_INLINE est défini.
#	La valeur de SysBase est réglée dans les startup code startup.o et minstart.o.
uae/dh0/Libraries-no-inline : Libraries.c
	@echo ===== COMPIL+LINKING avec fonction stubs de amiga.lib ======
	$(CC) +aos68km -g -v $< -D_NO_INLINE -lamiga -o $@ \
	      -I%VBCC%/targets/m68k-amigaos/include -I%VBCC%/NDK3.2R4/Include_H

uae/dh0/LibrariesAuto : LibrariesAuto.C
	@echo ============== COMPIL INLINE+LINKING Auto.lib ===============
	$(CC) +aos68k -g -v $< -lauto -o $@ \
	      -I%VBCC%/targets/m68k-amigaos/include -I%VBCC%/NDK3.2R4/Include_H

uae/dh0/RawDoFmt : RawDoFmt.C
	@echo ====================== COMPIL RawDoFmt ======================
	$(CC) +aos68km -g -v $< -o $@ \
	      -I%VBCC%/targets/m68k-amigaos/include -I%VBCC%/NDK3.2R4/Include_H

# Exemple d'utilisation de PTReplay.library avec génération de fichier include proto + inline :
subsystem:
	$(MAKE) -C PTReplayLib


$(ADF) : uae/dh0/Libraries uae/dh0/Libraries-no-inline uae/dh0/LibrariesAuto uae/dh0/RawDoFmt \
		 uae/dh0/s/Startup-Sequence subsystem
# Avec subsystem en dépendance, ça régénère à chaque fois l'ADF
	-$(RM) $(ADF)
	exe2adf --directory uae/dh0 --label $(notdir $(CURDIR)) --adf $(ADF)

clean:
	$(MAKE) -C PTReplayLib clean
	-$(RM) $(subst /,$(PATHSEP),uae/dh0/*)
	-$(RM) $(ADF)
	-$(RM) $(subst /,$(PATHSEP),$(UAE_CACHE_FILE))
	-$(RM) $(subst /,$(PATHSEP),$(MAKEFILE_UPTODATE))

# Force clean when Makefile is updated :
$(MAKEFILE_UPTODATE): Makefile
	make clean
	$(TOUCH) $(subst /,$(PATHSEP),$@)
