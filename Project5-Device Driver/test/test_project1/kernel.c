void __attribute__((section(".entry_function"))) _start(void)
{
	char *tx_fifo_write = (char *)0xffffffffbfe00000;
	char *tx_fifo_state = (char *)0xffffffffbfe00005;
	
	void (*printstring)(char *c) = (void *)0xffffffff8f0d5534;
	void (*printchar)(char c) = (void *)0xffffffff8f0d5570;
	
	(*printstring)("Hello OS!\n\r");
	(*printstring)("Ready for input!!!!!!!!\n\r");

	char outchar;
	while(1){
		if( (*tx_fifo_state) & 0x01 ){
			outchar = (*tx_fifo_write);
			if( outchar == '\r' ){
				(*printchar)('\r');
				(*printchar)('\n');
			}
			else
			(*printchar)(outchar);
		}
	}
}
