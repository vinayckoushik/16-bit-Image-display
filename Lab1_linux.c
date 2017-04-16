#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	Display			*Monitor;
	Window			ImageWindow;
	GC				ImageGC;
	XWindowAttributes		Atts;
	XImage			*Picture;
	int				ROWS,COLS,MAX,i;
	unsigned char	*displaydata;
	unsigned char	*img;
	unsigned char 	d[2];
	unsigned char	g1=0,g2=0,r1=0,b1=0;
	char header[80];
	/* ... */
	FILE *fpt;
	if ((fpt=fopen(argv[1],"rb")) == NULL)
	{
  		printf("Unable to open %s for reading\n",argv[1]);
  		exit(0);
	}
	fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&MAX );
	
  	header[79]=fgetc(fpt);
	
	
	if (strcmp(header,"P5") == 0)
	{
		img=(unsigned char *)malloc(ROWS*COLS);
		displaydata=(unsigned char *)malloc(ROWS*COLS);
		fread(img,1,ROWS*COLS,fpt);
		fclose(fpt);
	  for(i=0;i<ROWS*COLS;i++)
	  {
		g1=(img[i]>>3) & 31;
		g2=(img[i]>>2) & 63;
		d[0]=(g1<<3) + (g2>>3);
		d[1]=(g2<<5) + (g1);
		displaydata[i*2]=d[1];
		displaydata[(i*2)+1]=d[0];
	  }
	}
	
	else if(strcmp(header,"P6") == 0)
	{
		img=(unsigned char *)malloc(ROWS*COLS*3);
		displaydata=(unsigned char *)malloc(ROWS*COLS*2);
		fread(img,1,ROWS*COLS*3,fpt);
		fclose(fpt);

	for(i=0;i<ROWS*COLS;i++)
	{
		r1=(img[i*3]>>3) & 31;
		g1=(img[(i*3)+1]>>2) & 63;
		b1=(img[(i*3)+2]>>3) & 31;
		d[0]=(r1<<3) + (g1>>3);
		d[1]=(g1<<5) + (b1);			
		displaydata[i*2]=d[1];
		displaydata[(i*2)+1]=d[0];
	}
	}
	else
	{
  		printf("%s file is not PPM image\n",argv[1]);
  		fclose(fpt);
  		exit(0);
  	}

	Monitor=XOpenDisplay(NULL);
	if (Monitor == NULL)
  	{
  		printf("Unable to open graphics display\n");
  		exit(0);
  	}

	ImageWindow=XCreateSimpleWindow(Monitor,RootWindow(Monitor,0),
	500,100,		/* x,y on screen */
	COLS,ROWS,	/* width, height */
	2, 		/* border width */
	BlackPixel(Monitor,0),
	WhitePixel(Monitor,0));

	ImageGC=XCreateGC(Monitor,ImageWindow,0,NULL);

	XMapWindow(Monitor,ImageWindow);
	XFlush(Monitor);
	while(1)
  	{
  		XGetWindowAttributes(Monitor,ImageWindow,&Atts);
  		if (Atts.map_state == IsViewable /* 2 */)
    		break;
  	}

	Picture=XCreateImage(Monitor,DefaultVisual(Monitor,0),
		DefaultDepth(Monitor,0),
		ZPixmap,	/* format */
		0,		/* offset */
		displaydata,/* the data */
		COLS,ROWS,	/* size of the image data */
		16,		/* pixel quantum (8, 16 or 32) */
		0);		/* bytes per line (0 causes compute) */

	XPutImage(Monitor,ImageWindow,ImageGC,Picture,
		0,0,0,0,	/* src x,y and dest x,y offsets */
		COLS,ROWS);	/* size of the image data */

	XFlush(Monitor);
	sleep(2);
	XCloseDisplay(Monitor);
}

