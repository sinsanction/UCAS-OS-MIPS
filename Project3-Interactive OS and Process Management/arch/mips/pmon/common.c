#include "common.h"
#include "string.h"

void port_write_ch(char ch)
{
	typedef void (*FUNC_POINT)(char s);
	FUNC_POINT _printch = (FUNC_POINT)0xffffffff8f0d5570;
	_printch(ch);
}

void port_write(char *str)
{
	typedef void (*FUNC_POINT)(char *s);
	FUNC_POINT _printstr = (FUNC_POINT)0xffffffff8f0d5534;
	_printstr(str);
}
/*
void port_write_ch(char ch)
{
	// typedef void (*FUNC_POINT)(char s);
	// FUNC_POINT _printch = (FUNC_POINT)0x8007ba00;
	// _printch(ch);

	unsigned char *write_port = (unsigned char *)(PORT_URT2 + REG_DAT);
	unsigned char *stat_port = (unsigned char *)(PORT_URT2 + REG_LSR);

	if (ch == '\n')
	{
		while ((*stat_port & 0x20) == 0)
		{
		};
		*write_port = '\r';
	}

	while ((*stat_port & 0x20) == 0)
	{
	};
	*write_port = ch;
}

void port_write(char *str)
{
	// typedef void (*FUNC_POINT)(char *s);
	// FUNC_POINT _printstr = (FUNC_POINT)0x8007b980;
	// _printstr(str);

	int i, len = strlen(str);

	for (i = 0; i < len; i++)
	{
		port_write_ch(str[i]);
	}
}
*/
// use libepmon.a to read SD card
// sdread(char *buff, int offset, int size)
int sdwrite(unsigned char *buf, unsigned int base, int n)
{

	typedef int (*FUNC_POINT)(unsigned char *buf, unsigned int base, int n);
	FUNC_POINT new_sdcard_write = (FUNC_POINT)0xffffffff8f0d5d54; //(FUNC_POINT)0x80011080;
	//new_sdcard_write(base, n, buf);
	new_sdcard_write(buf, base, n);

	return (n);
}

int sdread(unsigned char *buf, unsigned int base, int n)
{
	typedef int (*FUNC_POINT)(unsigned char *buf, unsigned int base, int n);
	FUNC_POINT new_sdcard_read = (FUNC_POINT)0xffffffff8f0d5e10; //0x80011000;
	new_sdcard_read(buf, base, n);
	return (n);
}
void sd_card_read(void *dest, uint32_t offset, uint32_t size)
{
	sdread((char *)dest, offset, size);
}

// use libepmon.a to write SD card
// sdwrite(char *buff, int offset, int size)
void sd_card_write(void *dest, uint32_t offset, uint32_t size)
{
	// printk("write:0x%x, size:%d\n", offset, size);
	sdwrite((char *)dest, offset, size);
}
