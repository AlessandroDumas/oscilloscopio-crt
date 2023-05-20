

#include "main.h"
#include "xiaolin_wu_line.h"

#include <stdio.h>

#define PI 3.14159265358979323846
//double PI=4*atan(1.0);

//devono essere 2 numeri pari
#define XSIZE 128*5
#define YSIZE 128*4

// memoria quadro - immagine
//dimensione deve essere 2 numeri pari
uint16_t image[YSIZE][XSIZE];

uint16_t tanh_tab[65536];

typedef struct {
	int16_t x;
	int16_t y;
} coord_int;

typedef struct {
	float x;
	float y;
} coord_float;

struct draw_setup {
	float lum;
	uint8_t modo;
} draw_setup;

struct image_setup {
	uint16_t xsize;
	uint16_t ysize;
} image_setup;

//kernel di convoluzione
uint16_t matrice[3][3]={0, 1, 0, 1, 4, 1, 0, 1, 0};//crocicchio

/*
int cicli()
{
//scrive su stdout una sequenza di campioni S16 bit stereo (interlacciati)
//il segnale è il campionamento di 2 sinusoidi isofrequenziali
//la prima ha armoniche
//la seconda no
	unsigned int i;
	unsigned int t;
	int16_t xy[2];
	float phase;
	
	for(t=0; t<((5<<16)); t++)
	{
		phase=2*PI*t/65536*100.0;

		xy[0]=32768*pow(sin(phase),3);
		xy[1]=32768*sin(2*phase);
		
//		image[512+xy[0]>>6][512+xy[1]>>6]++;
		
		fwrite(xy, 4, 1, stdout);

	//	putw(x, stdout);
	//	putw(y, stdout);
	}
	
	return 0;
	
}
*/

//aggiunge un punto nell'immagine
//	centrato: (0,0) = centro immagine
int punto_c(float x, float y, float val)
{
	int16_t xx=image_setup.xsize/2+x;
	int16_t yy=image_setup.ysize/2+y;
	uint32_t vv;

	// non disegnarlo se al di fuori del quadro
	if(xx >= image_setup.xsize || xx < 0)
	{
//		fprintf(stderr, "x underflow %d\n",x);
		return -1;
	}
	if(yy >= image_setup.ysize || yy < 0 )
	{
//		fprintf(stderr, "y underflow %d\n",x);
		return -1;
	}
	

	if (draw_setup.modo & 0b0001)
	{
		//punto singolo somma la luce
		vv=image[yy][xx]+val*draw_setup.lum;
		if(vv>65535)	//satura
		{
			vv=65535;
	//		fprintf(stderr, "luma overflow %d,%d",xx,yy);
		}
		image[yy][xx]=vv;
	}

	if (draw_setup.modo & 0b0010)
	{
		//punto singolo moltiplica il valore
		vv=image[yy][xx]*val;
		if(vv>65535)	//satura
		{
			vv=65535;
	//		fprintf(stderr, "luma overflow %d,%d",xx,yy);
		}
		else if(vv<0)
		{
			vv=0;
	//		fprintf(stderr, "luma underflow %d,%d",xx,yy);
		}
		image[yy][xx]=vv;
	}

	if (draw_setup.modo & 0b0100)
	{
		//punto singolo sostituisci
		vv=val;
		if(vv>65535)	//satura
		{
			vv=65535;
	//		fprintf(stderr, "luma overflow %d,%d",xx,yy);
		}
		else if(vv<0)
		{
			vv=0;
	//		fprintf(stderr, "luma underflow %d,%d",xx,yy);
		}
		image[yy][xx]=vv;
	}

	/*
	//punto singolo non lineare
	vv=image[yy][xx]+val*draw_setup.lum;
	if(vv>65535)
		vv=65535;
	image[yy][xx]=tanh_tab[vv/3];
	*/
	
	return 0;
	
}


//disegna più punti per ogni punto
void punto_sparso(float x, float y, float val)
{

/*	
	//tipo convoluzione
	int xx,yy;
	for(xx=0;xx<3;xx++)
		for(yy=0;yy<3;yy++)
			punto(x+xx-1,y+yy-1,val*matrice[yy][xx]/5.0);
	
*/

	//con alea
	int i;
	float rx,ry;
	for(i=0;i<3;i++)
	{
		rx=(1-0.5*rand()/RAND_MAX);
		ry=(1-0.5*rand()/RAND_MAX);
		
		punto_c(x+1.4*rx*rx,y+1.4*ry*ry,val/2);
	}

}

void punto(float x, float y, float val)
{

	if(draw_setup.modo & 0b10000000)
		punto_sparso(x,y,val);
	else
		punto_c(x,y,val);
}

/*
coordinata leggibuffer(); 
{
	fread(stdin,)
}
*/


//linea
void linea(coord_float a, coord_float b)
{
	drawLine(a.x,a.y,b.x,b.y);
}


// disegna una griglia come un oscilloscopio
void griglia()
{
	//	|-|-|-|-|-|-|-|-|-|-|
	//	10x8
	//	11x9 linee
	
	uint16_t lG=(1<<10);	//luminosità del tratto
//	uint16_t lG=0.5;	//Moltiplicatore
	
	uint8_t n,m;
	coord_float a,b,offset;
	
	offset.x=image_setup.xsize/2;
	offset.y=image_setup.ysize/2;

	// verticali
	draw_setup.lum=1.0*lG;
	draw_setup.modo=0b0001;	//somma	
//	draw_setup.modo=0b0010;	//motliplica
//	draw_setup.modo=0b0100;	//sostituisci

	for(n=0;n<11;n++)
	{
		a.x=n/10.0*image_setup.xsize -offset.x;
		a.y=0 -offset.y;
		b.x=a.x;
		b.y=image_setup.ysize -offset.y;
		
		linea(a,b);
	}
	// subdivisoni verticali
	for(n=0;n<51;n++)
	{
		a.x=n/10.0/5.0*image_setup.xsize -offset.x;
		a.y=-image_setup.ysize/8.0/5.0/2.0;
		b.x=a.x;
		b.y=-a.y;
		
		linea(a,b);
	}

	// orizzontali
	for(m=0;m<9;m++)
	{
		a.x=0 -offset.x;
		a.y=m/8.0*image_setup.ysize -offset.y;
		b.x=image_setup.xsize -offset.x;
		b.y=a.y;

		linea(a,b);
	}
	// subdivisoni orizzontali
	for(m=0;m<41;m++)
	{
		a.x=-image_setup.xsize/10.0/5.0/2.0;
		a.y=m/8.0/5.0*image_setup.ysize -offset.y;
		b.x=-a.x;
		b.y=a.y;

		linea(a,b);
	}
	

	/*
	 * prova
	 *
	a.x=-200;
	a.y=100;
	b.x=-150;
	b.y=500;

	linea(a,b);
	*/
}


void sinusoide()
{
	
	int16_t x,y;
	float f=100;	//frequenza sinusoide
	float Tx=20e-3;	//periodo scansione
	float t;	//tempo
	
	while(t<Tx)
        {
		x=image_setup.xsize*t/Tx-image_setup.xsize/2;
		y=image_setup.ysize/2*sin(2*PI*f*t);
		
		punto(x,y,1);
		t+=Tx/(1<<16);
	}
}


void lissajous()
{

	int16_t x,y;
	int t;
	float phase;
        for(t=0; t<(1<<17); t++)
        {
		phase=2*PI*t/65536*100.0;

		x=(image_setup.xsize/4)*pow(sin(phase*7/16),3);
		y=(image_setup.ysize/4)*sin(2*phase);
		
		punto(x,y,1);
	}


}

//cancella immagine
void clear_img()
{
	int r; //riga
	int c; //col

	for(r=0;r<image_setup.ysize;r++)
		for(c=0;c<image_setup.xsize;c++)
			image[r][c]=0;	// cancella
}

//riempi immagine
void fill_img(float val)
{
	int r; //riga
	int c; //col

	for(r=0;r<image_setup.ysize;r++)
		for(c=0;c<image_setup.xsize;c++)
			image[r][c]=val;  // sostituisce
}

//affievolisce immagine (persistenza)
void fade_img()
{
	int r; //riga
	int c; //col

	for(r=0;r<image_setup.ysize;r++)
		for(c=0;c<image_setup.xsize;c++)
			image[r][c]=image[r][c]/2; //persistenza

}

//scrive immagine (raw) su stdout con gamma
void scrivi_img_gamma()
{

	uint16_t riga[XSIZE]; //spazio della riga "gammata"

	int r; //riga
	int x;

	//write
	for(r=0;r<image_setup.ysize;r++)
	{
//		fprintf(stderr, "riga %d\n",r);
		//applica gamma
		for(x=0;x<image_setup.xsize;x++)
			riga[x]=sqrt(1.0*image[r][x])*(1<<8);
		//scrivi riga
		fwrite(riga,2,image_setup.xsize,stdout);
	}
}

//scrive immagine (raw) su stdout
void scrivi_img()
{
	int r; //riga

	//write
	for(r=0;r<image_setup.ysize;r++)
	{
//		fprintf(stderr, "riga %d\n",r);
		fwrite(image[r],2,image_setup.xsize,stdout);
	}

}

void leggi_stdin()
{
//leggi da stdin	
	
	coord_float p,a,b;

	float scala=image_setup.ysize/65536.0;

	while(fread(&p,4,1,stdin)!=0)
	{

		//punti
//		punto(p.x*scala,p.y*scala,1);
		
		//linee
		a.x=p.x*scala;
		a.y=p.y*scala;
		linea(a,b);
		b.x=a.x;
		b.y=a.y;

	}

	
}

//
//riceve da stdin un flusso di campioni stereo int16
//disegna <punti per frame> linee
//manda su stdout un flusso di frame video (raw dati di image[])
//
void video()
{

	int punti_per_frame=(1<<11);
	//int punti_per_frame=44100/30;
	coord_int p;
	coord_float a,b;

	float dist;	//lunghezza tratto di linea
	float lt;	//luminosità dovuta al tempo trascorso
	int f=0;
	int i;
	int r;

	float scala=image_setup.ysize/65536.0;

	while(!feof(stdin))
	{

		fade_img();

		for(i=0;i<punti_per_frame;i++)
		{
		
			fread(&p,4,1,stdin);

			//punti
	//		punto(p.x*scala,p.y*scala,1);
			
			//linee
			a.x=p.x*scala;
			a.y=-p.y*scala;
			
			//calcola lunghezza del tratto -> tratto lungo => meno luninosità
			dist=sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));

//			if(dist==0) fprintf(stderr, "dist=0");

			//tratti iso-lumici
//			draw_setup.lum=1.0*(1<<10);
			//luminosità dipendente dalla lunghezza
//			draw_setup.lum=1.0*(1<<13)/(dist);
			//luminosità dip dalla lunghezza e dal "tempo"
			lt=pow(2.0,1.0*i/punti_per_frame-1.0);
//			if((i%256)==0) fprintf(stderr,"%d, %f\n",i,lt);
			draw_setup.lum=(1.0*(1<<13))/dist*lt;
			if(draw_setup.lum>=(1<<18))
				draw_setup.lum=(1<<18);
			//draw_setup.modo=0b10000001;	//somma con punti sparsi
			draw_setup.modo=0b00000001;	//somma singolo punto
			linea(a,b);
			
			b.x=a.x;
			b.y=a.y;

		}
		
		//disegna la griglia;
		griglia();

		scrivi_img();
//		scrivi_img_gamma();

		//debug numero frame
//		fprintf(stderr, "frame %d\n",f);
		f++;
	}
}

int main()
{

	image_setup.xsize=XSIZE;
	image_setup.ysize=YSIZE;
	draw_setup.lum=1;
	draw_setup.modo=0b0001;

	fprintf(stderr, "dimensione quadro: %d x %d\n",image_setup.xsize,image_setup.ysize);

	//riempe una tabella con funzione di trasferimento non lineare (per saturazione luce)
	int h;
	for(h=0;h<65536;h++)
		tanh_tab[h]=tanh(3.5*h/65536.0)*65536.0;
	//
	
/*
	printf("%f\n", 1.0*rand()/RAND_MAX);
	return 0;
*/

	//

//	griglia();
	
//	leggi_stdin();

//	lissajous();

//	sinusoide();	
	
//	scrivi_img();

	video();
	
	return 0;
	
}


