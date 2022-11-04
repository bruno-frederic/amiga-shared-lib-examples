# Compile l'exemple SmallPlay de PTLibrary après avoir recréé avec fd2pragma les fichiers headers
# de la librarie à partir de ptreplay_lib.fd et clib/ptreplay_protos.h

CC=vc
CFLAGS=+kick13 -g -Iinclude/
#LDFLAGS=-lamiga
ODIR=build-vbcc
EXE=../uae/dh0/SmallPlay

# Prepare variables for target 'clean'
ifeq ($(OS),Windows_NT)
	RM:=del /F /Q
	TOUCH:=COPY /Y NUL
	PATHSEP:=\\
else
	RM:=rm -f
	TOUCH:=touch
	PATHSEP:=/
endif

all: $(ODIR)/Makefile.uptodate $(EXE) ../uae/dh0/ptreplay.library ../uae/dh0/RED.MOD

$(EXE) : SmallPlay.c $(ODIR)/ptreplay.lib include/proto/ptreplay.h \
										  include/inline/ptreplay_protos.h include/inline/ptreplay.h
	@echo ========================= SmallPlay =======================
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $(EXE)

# Recopie aussi les fichiers necessaires vers arbo UAE :
../uae/dh0/ptreplay.library : ptreplay.library
	COPY /Y ptreplay.library	$(subst /,$(PATHSEP),../uae/dh0/)

../uae/dh0/RED.MOD : RED.MOD
	COPY /Y RED.MOD				$(subst /,$(PATHSEP),../uae/dh0/)


# Génère les fichiers headers pour la lib PTreplay

# Topic sur cette lib avec GCC et génération de proto inline :
# http://eab.abime.net/showthread.php?t=104400
FD=$(CURDIR)/include/fd/ptreplay_lib.fd
# Note : se positionner dans le dossier de fd2pragma pour qu'il trouve son fichier fd2pragma.types


# Linker .lib library with stub functions:
# 12 = stub-functions for C - link library
$(ODIR)/ptreplay.lib : $(FD) include/clib/ptreplay_protos.h
	@echo ======================= Protos headers =====================
	cd ../bin && fd2pragma --infile $(FD) --special 12 --hunkname CODE --to $(CURDIR)/$(ODIR)

# Proto header file:
include/proto/ptreplay.h : $(FD) include/clib/ptreplay_protos.h
	cd ../bin && fd2pragma --infile $(FD) --special 38 --to $(CURDIR)/include/proto/
# 38 = proto file for all compilers (VBCC inline)

# Header file with VBCC assembler inline calls:
include/inline/ptreplay_protos.h : $(FD) include/clib/ptreplay_protos.h
	cd ../bin && fd2pragma --infile $(FD) --clib $(CURDIR)/include/clib/ptreplay_protos.h \
	          --special 70 --voidbase --to $(CURDIR)/include/inline/
# 70 = VBCC inline files

# Header file with GCC assembler inline calls:
include/inline/ptreplay.h : $(FD) include/clib/ptreplay_protos.h
	cd ../bin && fd2pragma --infile $(FD) --clib $(CURDIR)/include/clib/ptreplay_protos.h \
	          --special 40 --voidbase --to $(CURDIR)/include/inline/
# 40 = GCC inline file (preprocessor based)


clean:
	-$(RM) $(subst /,$(PATHSEP),$(ODIR)/ptreplay.lib)
	-$(RM) $(subst /,$(PATHSEP),include/proto/*.h)
	-$(RM) $(subst /,$(PATHSEP),include/inline/*.h)
	-$(RM) $(subst /,$(PATHSEP),$(ODIR)/Makefile.uptodate)

# Force clean when Makefile is updated :
$(ODIR)/Makefile.uptodate: Makefile
	make clean
	$(TOUCH) $(subst /,$(PATHSEP),$@)
