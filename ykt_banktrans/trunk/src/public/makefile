include $(HOME)/bank/src/mak/mkstand.mak
TARGET=libbccpub.a
LIBDIR=../../lib

SOURCE= \
express\
mutex\
mypub\
mac\
profile\
pubfunc\
d3des\
Base64\
md5\
yktclt\
profile


# =====================================================
all:$(TARGET)
	mv $(TARGET) $(LIBDIR)
	@echo $(TARGET) is moved to $(LIBDIR)
	
clean:
	@./.clean
	rm -f *.o $(TARGET)


$(TARGET):$(SOURCE:=.o)  
	$(AR) $(ARFLAGS) $@ $(SOURCE:=.o)
