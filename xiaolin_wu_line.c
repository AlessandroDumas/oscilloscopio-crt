// https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm

#include "main.h"
#include "xiaolin_wu_line.h"

#include <stdio.h>

void plot(float x, float y, float c)
{
	//plot the pixel at (x, y) with brightness c (where 0 ≤ c ≤ 1)
	punto(x,y,c);
//	punto_sparso(x,y,c);
//	printf("x=%f, y=%f, c=%f\n",x,y,c);
}
/*
void swap(int *i, int *j)
{
	int t = *i;
	*i = *j;
	*j = t;
}
*/
void swap(float *i, float *j)
{
	float t = *i;
	*i = *j;
	*j = t;
}


// integer part of x
int ipart(float x)
{
	return floor(x);
}

/*
int round(float x)
{
	return ipart(x + 0.5);
}
*/

// fractional part of x
float fpart(float x)
{
	return x - floor(x);
}

float rfpart(float x)
{
	return 1 - fpart(x);
}

void drawLine(float x0, float y0, float x1, float y1)
{
	_Bool steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep)
	{
		swap(&x0, &y0);
		swap(&x1, &y1);
	}
	if (x0 > x1)
	{
		swap(&x0, &x1);
		swap(&y0, &y1);
	}

//	printf("(%d,%d), (%d,%d);\n",x0,y0,x1,y1);

	float dx = x1 - x0;
	float dy = y1 - y0;
	
	float gradient = dy / dx;
	if (dx == 0.0)
	{
		gradient = 1.0;
	}
	
//	printf("gradient=%f\n",gradient);

	// handle first endpoint
	float xend = round(x0);
	float yend = y0 + gradient * (xend - x0);

	float xgap = rfpart(x0 + 0.5);
	float xpxl1 = xend; // this will be used in the main loop
	float ypxl1 = ipart(yend);

	if (steep)
	{
		plot(ypxl1,   xpxl1, rfpart(yend) * xgap);
		plot(ypxl1+1, xpxl1,  fpart(yend) * xgap);
	}
	else
	{
		plot(xpxl1, ypxl1  , rfpart(yend) * xgap);
		plot(xpxl1, ypxl1+1,  fpart(yend) * xgap);
	}

	float intery = yend + gradient; // first y-intersection for the main loop
    
	// handle second endpoint
	xend = round(x1);
	yend = y1 + gradient * (xend - x1);
	xgap = fpart(x1 + 0.5);
	float xpxl2 = xend; //this will be used in the main loop
	float ypxl2 = ipart(yend);

	if (steep)
	{
		plot(ypxl2  , xpxl2, rfpart(yend) * xgap);
		plot(ypxl2+1, xpxl2,  fpart(yend) * xgap);
	}
	else
	{
		plot(xpxl2, ypxl2,  rfpart(yend) * xgap);
		plot(xpxl2, ypxl2+1, fpart(yend) * xgap);
	}

	float x;
	// main loop
	if (steep)
	{
		for (x = xpxl1 + 1; x < xpxl2; x++)
		{
			plot(ipart(intery)  , x, rfpart(intery));
			plot(ipart(intery)+1, x,  fpart(intery));
			intery = intery + gradient;
		}
	}
	else
	{
		for (x = xpxl1 + 1; x < xpxl2; x++)
		{
			plot(x, ipart(intery),  rfpart(intery));
			plot(x, ipart(intery)+1, fpart(intery));
			intery = intery + gradient;
		}
	}

} // end function


