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
 
#include "LedMatrix.h"

LedMatrix::LedMatrix(char din, char load, char clk, char width, char height) 
{
	if (width >= 8 && width <= MAX_COLS && height >=8 && height <= MAX_ROWS)
	{
		m_cnRows	= height;
		m_cnCols	= width;
		m_cnDeviceR	= m_cnRows / 8;
		m_cnDeviceC	= m_cnCols / 8;
		m_cnDeviceT	= m_cnDeviceR * m_cnDeviceC; 
		m_din		= din;
		m_load		= load;
		m_clk		= clk;

		pinMode(m_din, OUTPUT);
		pinMode(m_load,  OUTPUT);
		pinMode(m_clk, OUTPUT);

		digitalWrite(m_load, HIGH);
		digitalWrite(m_din, LOW);
		digitalWrite(m_clk, LOW);

		digitalWrite(m_load, LOW);
		for(char i=0; i<m_cnDeviceT; i++)
		{
			write(REG_TEST, OP_NOTEST);
		}
		digitalWrite(m_load, HIGH);

		digitalWrite(m_load, LOW);
		for(char i=0; i<m_cnDeviceT; i++)
		{
			write(REG_MODE, OP_GRAPHIC);
		}
		digitalWrite(m_load, HIGH);

		digitalWrite(m_load, LOW);
		for(char i=0; i<m_cnDeviceT; i++)
		{
			write(REG_SCAN, OP_SCANALL);
		}
		digitalWrite(m_load, HIGH);
	}
}

void LedMatrix::write(char addr, char data)
{
	char i = 0;

	for(i = 0; i < 8; i++)
	{
		digitalWrite(m_clk, LOW);
		if (addr & 0x80)
		{
			digitalWrite(m_din, HIGH);
		}
		else
		{
			digitalWrite(m_din, LOW);
		}
		digitalWrite(m_clk, HIGH);
		addr = addr << 1;
	}

	for(i = 0; i < 8; i++)
	{
		digitalWrite(m_clk, LOW);
		if (data & 0x80)
		{
			digitalWrite(m_din, HIGH);
		}
		else
		{
			digitalWrite(m_din, LOW);
		}
		digitalWrite(m_clk, HIGH);	
		data = data << 1;
	}	
}

char LedMatrix::getHeight() 
{
    return m_cnRows;
}

char LedMatrix::getWidth() 
{
    return m_cnCols;
}

//
// initialize and start display
//
void LedMatrix::init(char intensity)
{
	digitalWrite(m_load, LOW);
	for(char i=0; i<m_cnDeviceT; i++)
	{
		write(REG_SHUTDOWN, 0x01);
	}
	digitalWrite(m_load, HIGH);	

	digitalWrite(m_load, LOW);
	for(char i=0; i<m_cnDeviceT; i++)
	{
		write(REG_INTENSITY, intensity);
	}	
	digitalWrite(m_load, HIGH);
}

//
// shut down display
//
void LedMatrix::stop()
{
	digitalWrite(m_load, LOW);
	for(char i=0; i<m_cnDeviceT; i++)
	{
		write(REG_SHUTDOWN, 0x00);
	}
	digitalWrite(m_load, HIGH);	
}

//
// blanking display is as simple as setting zeroes
// to display buffer
//
void LedMatrix::blank() 
{
	for(char r = 0; r < m_cnRows; r++)
	{
		for(char c = 0; c < m_cnDeviceC; c++)
		{
			m_arDisplay[r][c] = 0;
		}
	}
}

//
// this is the routine that actually updates hardware
// based on display buffer, it should be called
// after all updates to display buffer are done
//
void LedMatrix::update()
{
	char addr	= 0;
	char data	= 0;
	char row	= 0;
	char col	= 0;
	char dev	= m_cnDeviceT;
	
	for(addr = 0; addr < 8; addr++)
	{
		digitalWrite(m_load, LOW);
		dev		= m_cnDeviceT;
		while(dev)
		{
			row		= (dev - 1) / m_cnDeviceC;
			row		= row << 3;
			row		= row + addr;
			col		= (dev - 1) % m_cnDeviceR;
			data	= m_arDisplay[row][col];

			write(REG_D0 + addr, data);
			dev--;
		}
		digitalWrite(m_load, HIGH);
	}		
}

//
// drawing a rectangle whose opposite corners are (sx, sy) and (ex, ey) 
// inclusive
//
void LedMatrix::rectangle(char sx, char sy, char ex, char ey, char opcode)
{
	char tmp;

	if (sx > ex)
	{
		tmp = ex;
		ex	= sx;
		sx	= tmp;
	}

	if (sy > ey)
	{
		tmp = ey;
		ey	= sy;
		sy	= tmp;
	}

	for(char x = sx; x <= ex; x++)
	{
		setPixel(x, sy, opcode);
	}
	for(char x = sx; x <= ex; x++)
	{
		setPixel(x, ey, opcode);
	}
	for(char y = sy+1; y < ey; y++)
	{
		setPixel(sx, y, opcode);
	}
	for(char y = sy+1; y < ey; y++)
	{
		setPixel(ex, y, opcode);
	}
}

//
// draw a ling between (sx, sy) and (ex, ey)
// using Bresenham's line algorithm using integers only
//
// http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
// 
void LedMatrix::line(char sx, char sy, char ex, char ey, char opcode)
{
	char tmp;

	if (sx > ex)
	{
		tmp = ex;
		ex	= sx;
		sx	= tmp;
	}

	if (sy > ey)
	{
		tmp = ey;
		ey	= sy;
		sy	= tmp;
	}

	int	dx	= ex - sx;
	int dy	= ey - sy;
	int D	= dy+dy-dx;
	setPixel(sx, sy, opcode);

	char	y = sy;
	for(char x = sx+1; x <= ex; x++)
	{
		if (D > 0)
		{
			y++;
			setPixel(x, y, opcode);
			D = D + (dy+dy-dx-dx);
		}
		else
		{
			setPixel(x, y, opcode);
			D = D + dy + dy;
		}
	}
}

//
// draw a circle with center (sx, sy) and radius r
// using Bresenham's circle algorithm using integers only
//
// http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
//
void LedMatrix::circle(char x0, char y0, char radius, char opcode)
{
	char	x			= radius;
	char	y			= 0;
	int		xChange		= 1 - (radius << 1);
	int		yChange		= 0;
	int		radiusError = 0;

	while(x >= y)
	{
		setPixel(x + x0, y + y0, opcode);
		setPixel(y + x0, x + y0, opcode);
		setPixel(-x + x0, y + y0, opcode);
		setPixel(-y + x0, x + y0, opcode);
		setPixel(-x + x0, -y + y0, opcode);
		setPixel(-y + x0, -x + y0, opcode);
		setPixel(x + x0, -y + y0, opcode);
		setPixel(y + x0, -x + y0, opcode);

		y++;
		radiusError += yChange;
		yChange += 2;
		if(((radiusError << 1) + xChange) > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

//
// scroll display UP, DOWN, LEFT or RIGHT by one pixel
// to scroll multiple pixels, call this multiple times
// it is still fast because all operation is done in
// memory, no IO operation at all
//
void LedMatrix::scroll(char direction, char fill)
{
	char row = 0;
	char col = 0;
	char ucTmp = 0;
	char ucNxt = 0;

	switch(direction)
	{
	case SCROLL_LEFT:
		for(col = 0; col < m_cnDeviceC-1; col++)
		{
			for(row = 0; row < m_cnRows; row++)
			{
				ucTmp = m_arDisplay[row][col];
				ucTmp = ucTmp << 1;				
				ucNxt = m_arDisplay[row][col+1];
				if (ucNxt & 0x80)
				{
					ucTmp |= 1;
				}
				m_arDisplay[row][col] = ucTmp;
			}
		}

		col = m_cnDeviceC - 1;
		for(row = 0; row < m_cnRows; row++)
		{
			ucTmp = m_arDisplay[row][col];
			ucTmp = ucTmp << 1;				
			if (fill)
			{
				ucTmp |= 1;
			}
			m_arDisplay[row][col] = ucTmp;
		}
		break;
	case SCROLL_RIGHT:
		for(col = m_cnDeviceC-1; col > 0; col--)
		{
			for(row = 0; row < m_cnRows; row++)
			{
				ucTmp = m_arDisplay[row][col];
				ucTmp = ucTmp >> 1;				
				ucNxt = m_arDisplay[row][col-1];
				if (ucNxt & 0x01)
				{
					ucTmp |= 0x80;
				}
				m_arDisplay[row][col] = ucTmp;
			}
		}

		col = 0;
		for(row = 0; row < m_cnRows; row++)
		{
			ucTmp = m_arDisplay[row][col];
			ucTmp = ucTmp >> 1;				
			if (fill)
			{
				ucTmp |= 0x80;
			}
			m_arDisplay[row][col] = ucTmp;
		}
		break;
	case SCROLL_UP:
		for(row = 0; row < m_cnRows-1; row++)
		{
			for(col = 0; col < m_cnDeviceC; col++)
			{
				m_arDisplay[row][col] = m_arDisplay[row+1][col];
			}
		}	
		row = m_cnRows-1;
		for(col = 0; col < m_cnDeviceC; col++)
		{
			m_arDisplay[row][col] = fill;
		}
		break;
	case SCROLL_DOWN:
		for(row = m_cnRows - 1; row > 0; row--)
		{
			for(col = 0; col < m_cnDeviceC; col++)
			{
				m_arDisplay[row][col] = m_arDisplay[row-1][col];
			}
		}
		row = 0;
		for(col = 0; col < m_cnDeviceC; col++)
		{
			m_arDisplay[row][col] = fill;
		}
		break;
	}
}

//
// draw a sprite pattern at location (x, y) where the upper left corner of the sprite
// is drawn at.
//
bool LedMatrix::sprite(char x, char y, const char *sprite, unsigned int count, char opcode)
{
	bool	rval		= false;
	char	ucW			= sprite[0];
	char	ucH			= sprite[1];
	char	ucTemp		= 0;
	char	ucMask		= 0;
	char	ucScanline	= 1;

	if (ucW < MAX_COLS && ucH < MAX_ROWS)
	{
		rval		= true;
		ucScanline	= ucW / 8;
		if (ucW % 8)
		{
			ucScanline++;
		}

		unsigned int uiC = (int) (ucScanline) * (int) ucH + 2;
		if (uiC <= count)
		{
			rval	= true;
			sprite += 2;	// skip first two bytes as they are width and height
			for(char r = 0; r < ucH; r++)
			{
				if (r + y < m_cnRows)
				{
					for(char c = 0; c < ucW; c++)
					{
						if ((c % 8) == 0)
						{
							ucTemp = *sprite;
							sprite++;
						}

						switch(opcode)
						{
						case GRAPHIC_DRAW_OFF:
							setPixel(c+x, r+y, GRAPHIC_DRAW_OFF);
							break;
						case GRAPHIC_DRAW_SRC:
							ucMask = 0x80 >> (c % 8);
							if (ucTemp & ucMask)
							{
								setPixel(c+x, r+y, GRAPHIC_DRAW_SRC);
							}
							else
							{
								setPixel(c+x, r+y, GRAPHIC_DRAW_OFF);
							}
							break;
						case GRAPHIC_DRAW_XOR:
							setPixel(c+x, r+y, opcode);
							break;
						}
					}
				}
			}
		}
	}

	return	rval;
}

// 
// set a pixel value at location (x, y) based on opcode
//
void LedMatrix::setPixel(char x, char y, char opcode) 
{
	if (x>=0 && y>=0 && x < m_cnCols && y < m_cnRows)
	{
		char c = x / 8;
		char m = 0x80 >> (x % 8);
		switch(opcode)
		{
		case GRAPHIC_DRAW_OFF:
			m = ~m;
			m_arDisplay[y][c] &= m;
			break;
		case GRAPHIC_DRAW_SRC:
			m_arDisplay[y][c] |= m;
			break;
		case GRAPHIC_DRAW_XOR:
			m = m_arDisplay[y][c] & m;
			m_arDisplay[y][c] ^= m;
			break;
		}
	}
}

//
// get pixel value at location (x, y)
//
char LedMatrix::getPixel(char x, char y) 
{
	char	rval = -1;
	if (x>=0 && y>=0 && x < m_cnCols && y < m_cnRows)
	{
		char c	= x / 8;
		char m	= 0x80 >> (x % 8);
		if (m_arDisplay[y][c] & m)
		{
			rval = 0x01;
		}
		else
		{
			rval = 0x00;
		}
	}
	return	0;
}
