
# To run, enter make or make all

all: sleepingTA

sleepingTA:
	gcc  -lpthread -o sleepingTA sleepingTA.c
