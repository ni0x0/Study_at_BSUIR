#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>


void interrupt(*prevKlavHandler)(...);	
void interrupt newKlavHandler(...);

void interrupt_init();
void blink();
void send_command(unsigned int comm);

char f_exit = 0;

int main() {

	blink();
	printf("\nPress ESC to exit program");
	interrupt_init();

	while (!f_exit) {
		char c = getch();
	}

	send_command(0xED);
	send_command(0);
	setvect(0x09, prevKlavHandler);
	return 0;
}

void interrupt newKlavHandler(...) {
	char buff[5];

	unsigned char c = inp(0x60); 
	if (c == 0x01) 
		f_exit = 1;

	itoa(c, buff, 16);
	cputs("\n\n\r0x60 port content: ");
	cputs(buff);

	cputs("\n\r0x64 port content: ");
	c = inp(0x64);
	char buff2[5];
	itoa(c, buff2, 16);
	cputs(buff2);

	(*prevKlavHandler)();
}

void interrupt_init() {
	disable();

	prevKlavHandler = getvect(0x09);
	setvect(0x09, newKlavHandler);

	enable();
}



void blink() {
	for (int i = 0; i < 3; i++) {
		send_command(0xED);
		send_command(7);
		delay(700);
		send_command(0xED);
		send_command(0);
		delay(400);
	}
	send_command(0xED);
	send_command(7);
}

void send_command(unsigned int comm) {
	unsigned int cond;
	unsigned char k_brd_is_free = 0;

	cond = inp(0x64);
	cond &= 2;

	if (cond != 0) {
		printf("\nERROR. Keyboard is busy\n");
		exit(-1);
	}

	for (int i = 0; i < 3; i++) {
		outp(0x60, comm);
		if (inp(0x60) != 0xFE) {
			k_brd_is_free = 1;
			break;
		}
	}

	if (k_brd_is_free == 0) {
		printf("\nERROR. Byte send failed\n");
		exit(-1);
	}

}
