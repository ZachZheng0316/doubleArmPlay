TARGET		= libdxl.a
OBJS		= dxl_hal.o dynamixel.o dmath.o MX28AT.o joint.o action.o demo.o algorithm.o playChess.o beng.o serialCommuni.o
SRCS		= $(OBJS:.o=.c)
INCLUDEDIRS	+= -I../include
LIBDIRS		+=
CFLAGS		= $(INCLUDEDIRS) -W -Wall -O2

#CC			= gcc
#AR			= ar
CC			= arm-linux-gnueabi-gcc
AR			= arm-linux-gnueabi-ar

$(TARGET): $(OBJS)
	$(AR) rs $@ $^
	cp $(TARGET) ../lib

.c.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "file deleted."

dep:
	gccmakedep $(SRCS)
