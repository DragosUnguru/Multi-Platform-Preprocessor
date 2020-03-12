CC = cl
OBJ_LIST = utils.obj list.obj hashmap.obj so-cpp.obj
CFLAGS   = /MD
EXE_NAME = so-cpp.exe

all : $(EXE_NAME)

so-cpp.exe : $(OBJ_LIST)
  $(CC) $(CFLAGS) /Fe$@ $**

so-cpp.o: so-cpp.c
	$(CC) $(CFLAGS) /c $<

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) /c $<

hashmap.o: hashmap.c hashmap.h
	$(CC) $(CFLAGS) /c $<

list.o: list.c list.h
	$(CC) $(CFLAGS) /c $<

clean :
  del *.obj $(EXE_NAME)