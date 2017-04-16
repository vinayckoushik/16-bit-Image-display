#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LRESULT CALLBACK EventProcessor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return(DefWindowProc(hWnd, uMsg, wParam, lParam));
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine, int nCmdShow)
{
	WNDCLASS		wc;
	HWND			WindowHandle;
	int			ROWS, COLS, MAX, i;
	unsigned char	*displaydata;
	unsigned char	*img;
	unsigned char 	d[2];
	unsigned char 	g1 = 0,r1=0,b1=0;
	char header[80];
	BITMAPINFOHEADER	bm_headerinfo;
	BITMAPINFO		*bm_info;
	HDC			hDC;
	FILE		*fpt;

	if( (fpt = fopen(lpCmdLine,"r+b")) == NULL)
	{

		MessageBox(NULL,"failed to open","WinMain",MB_OK);
	}
	fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &MAX);
	
	if (strcmp(header,"P5")== 0)
	{
		header[79] = fgetc(fpt);
		img = (unsigned char *)malloc(ROWS*COLS);
		displaydata = (unsigned char *)malloc(ROWS*COLS*2);
		fread(img, 1, ROWS*COLS, fpt);
		fclose(fpt);
		for (i = 0; i<ROWS*COLS; i++)
		{
			g1 = (img[i] >> 3) & 31;
			d[0] = (g1 << 2) + (g1 >> 3);
			d[1] = (g1 << 5) + g1 ;
			displaydata[i*2] = d[1];
			displaydata[(i*2) + 1] = d[0];
		}
	}
	else if((strcmp(header,"P6")== 0))
	{
		header[79] = fgetc(fpt);
		img = (unsigned char *)malloc(ROWS*COLS*3);
		displaydata = (unsigned char *)malloc(ROWS*COLS*2);
		fread(img, 1, ROWS*COLS*3, fpt);
		fclose(fpt);

		for(i=0;i<ROWS*COLS;i++)
		{
			r1=(img[i*3]>>3) & 31;
			g1=(img[(i*3)+1]>>3) & 31;
			b1=(img[(i*3)+2]>>3) & 31;
			d[0]=(r1<<2) + (g1>>3);
			d[1]=(g1<<5) + (b1);			
			displaydata[i*2]=d[1];
			displaydata[(i*2)+1]=d[0];

		}
	}
	else
	{
		printf("%s file is not PPM image\n",lpCmdLine);
  		fclose(fpt);
  		exit(0);
	}


	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)EventProcessor;
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "Image Window Class";
	if (RegisterClass(&wc) == 0)
		exit(0);

	WindowHandle = CreateWindow("Image Window Class", "ECE468 Lab1",
		WS_OVERLAPPEDWINDOW,
		10, 10, COLS, ROWS,
		NULL, NULL, hInstance, NULL);
	if (WindowHandle == NULL)
	{
		MessageBox(NULL, "No window", "Try again", MB_OK | MB_APPLMODAL);
		exit(0);
	}
	ShowWindow(WindowHandle, SW_SHOWNORMAL);

	bm_info = (BITMAPINFO *)calloc(1, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	hDC = GetDC(WindowHandle);

	/* ... set up bmiHeader field of bm_info ... */

	bm_headerinfo.biSize=sizeof(BITMAPINFOHEADER); 
	bm_headerinfo.biWidth=COLS;
	bm_headerinfo.biHeight=-ROWS; 
	bm_headerinfo.biPlanes=1;
	bm_headerinfo.biBitCount=16; 
	bm_headerinfo.biCompression=BI_RGB; 
	bm_headerinfo.biSizeImage=0; 
	bm_headerinfo.biXPelsPerMeter=0; 
	bm_headerinfo.biYPelsPerMeter=0;
	bm_headerinfo.biClrUsed=256;
	bm_headerinfo.biClrImportant=256;
	bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
	bm_info->bmiHeader=bm_headerinfo;

	for (i = 0; i<256; i++)	/* colormap */
	{
		bm_info->bmiColors[i].rgbBlue = bm_info->bmiColors[i].rgbGreen = bm_info->bmiColors[i].rgbRed = i;
		bm_info->bmiColors[i].rgbReserved = 0;
	}
	SetDIBitsToDevice(hDC, 0, 0, COLS, ROWS, 0, 0,
		0, /* first scan line */
		ROWS, /* number of scan lines */
		displaydata, bm_info, DIB_RGB_COLORS);
	ReleaseDC(WindowHandle, hDC);
	free(bm_info);
	MessageBox(NULL, "Press OK to continue", "", MB_OK | MB_APPLMODAL);
}
