PREFIX  = /usr/local
CFLAGS  = 
LDFLAGS = -lpthread

CC = gcc
RM = rm -f
INSTALL_PROG = install -m 755 -s

EXE = main

OBJS = thread.o main.o liste.o chargement_fichier.o

ALL : $(EXE)

thread.o : struct.h thread.h thread.c
	$(CC) -c thread.c $(CFLAGS) -o thread.o

main.o : chargement_fichier.h thread.h main.c
	$(CC) -c main.c $(CFLAGS) -o main.o

liste.o : struct.h liste.h liste.c
	$(CC) -c liste.c $(CFLAGS) -o liste.o

chargement_fichier.o : liste.h chargement_fichier.h chargement_fichier.c
	$(CC) -c chargement_fichier.c $(CFLAGS) -o chargement_fichier.o

$(EXE) : $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(LDFLAGS)

install : $(EXE)
	$(INSTALL_PROG) $(EXE) $(PREFIX)/bin

uninstall :
	$(RM) $(PREFIX)/bin/$(EXE)

clean :
	$(RM) $(OBJS) $(EXE)