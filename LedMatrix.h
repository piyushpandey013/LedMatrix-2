//
//    LedMatrix.cpp - A library for controlling 8x8 Led Matrix using 
//    MAX7219/MAX7221 as driver
//
//    Copyright (c) 2013 Peter Y Lin
// 
//    Permission is hereby granted, free of charge, to any person
//    obtaining a copy of this software and associated documentation
//    files (the "Software"), to deal in the Software without
//    restriction, including without limitation the rights to use,
//    copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the
//    Software is furnished to do so, subject to the following
//    conditions:
// 
//    This permission notice shall be included in all copies or 
//    substantial portions of the Software.
// 
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//    OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef LedMatrix_h
#define LedMatrix_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

//the opcodes for the MAX7221 and MAX7219
#define	REG_NOOP			0x00
#define REG_D0				0x01
#define REG_D1				0x02
#define REG_D2				0x03
#define REG_D3				0x04
#define REG_D4				0x05
#define REG_D5				0x06
#define REG_D6				0x07
#define REG_D7				0x08

#define	REG_MODE			0x09
#define	REG_INTENSITY		0x0A
#define REG_SCAN			0x0B
#define REG_SHUTDOWN		0x0C
#define REG_TEST			0x0F

#define	MAX_ROWS			96
#define	MAX_COLS			96

#define	OP_GRAPHIC			0x00
#define	OP_NOTEST			0x00
#define OP_SCANALL			0x07

#define	GRAPHIC_DRAW_OFF	0x00
#define GRAPHIC_DRAW_SRC	0x01
#define GRAPHIC_DRAW_XOR	0x02

#define SCROLL_LEFT			0
#define SCROLL_RIGHT		1
#define SCROLL_UP			2
#define	SCROLL_DOWN			3

class LedMatrix 
{
public:

	//
	// DIN			pin on the Arduino where data gets shifted out
    // CLK			pin for the clock
    // LOAD/CS		pin for selecting the device 
    // nRow			number of rows
	// nCol			number of columns 
	//
    LedMatrix(char din, char load, char clk, char width=8, char height=8);

	// return number of rows
	char getHeight();
	// return number of cols
	char getWidth();

	// initialize and start display
	void init(char intensity);
	// stop display
	void stop();
	// blank out display
	void blank();
	// update -- write buffer to devices
	void update();

	// scroll vertically, negative up, positive down
	void scroll(char direction, char fill=0);

	// draw a line
	void line(char sx, char sy, char ex, char ey, char opcode);
	// draw a circle
	void circle(char x0, char y0, char radius, char opcode);
	// draw a rectangle
	void rectangle(char sx, char sy, char ex, char ey, char opcode);
	// draw a sprite around
	bool sprite(char x, char y, const char *sprite, unsigned int count, char opcode);

	// set LED on/off
	void setPixel(char row, char col, char opcode);
	// get LED status
	char getPixel(char row, char col);
	// copy display buffer
	void setBuffer(char* pBuffer, char nBuffer);
	// get display buffer
	void getBuffer(char* pBuffer, char nBuffer);

private:

	void	write(char addr, char data);

	char	m_cnRows;
	char	m_cnCols;
	char	m_cnDeviceR;
	char	m_cnDeviceC;
	char	m_cnDeviceT;

	char	m_din;
	char	m_clk;
	char	m_load;
	char	m_arDisplay[MAX_ROWS][MAX_COLS/8];
};

#endif	//LedMatrix.h

