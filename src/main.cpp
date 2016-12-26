// (C) Kattanek
// 08.11.2014

#include <time.h>
#include <math.h>
#include <iostream>
using namespace std;

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_gfxPrimitives.h>
#undef main

#include "komplex.h"

#define SCREEN_XW 800
#define SCREEN_YW 600
#define MAX_THREADS 16

struct FLOAT_COLOR
{
    float r,g,b;
};

struct FRACTAL_PARAMETER
{
    int screen_xw,screen_yw;
    long double x1,y1,x2,y2;
    unsigned int max_iteration;
    int color_point_tab_len;
    FLOAT_COLOR *color_point_tab;
    unsigned int *color_tab;
};

struct THREAD_PARAM
{
	SDL_mutex *mutex;
	int AktZeile;
    SDL_Surface *screen;
    FRACTAL_PARAMETER *fractal_parameter;
    bool PictureIsFinish;
};

int mousepos_x, mousepos_y;

/// Funktion Deklaration ///
int Thread1 (void *p);
void StartNewFrame(THREAD_PARAM *param,SDL_Thread **threads , SDL_Surface *screen);
void CalcZeile(unsigned int *buffer, int y, FRACTAL_PARAMETER *fractal_param);
unsigned int CalcColor(long double x, long double y, int max_iteration, unsigned int *color_tab);
void SetNewFrame(int x1, int y1, int x2, int y2, FRACTAL_PARAMETER *fractal_param);
void SetColorTab(unsigned int *color_tab, int max_iteration, FLOAT_COLOR *color_point_tab, int color_point_tab_len);

int main(void)
{
    long double screen_ratio;

    bool SelectMode = false;
    bool FilterIsOn = false;
    int SelectModeX1,SelectModeY1,SelectModeX2,SelectModeY2;

    FLOAT_COLOR Colors[]={{0,0,1},{1,0,0},{1,1,0},{0,1,0},{0,1,1}};
    //FLOAT_COLOR Colors[]={{0.0,0.0,0.5},{1,1,1},{0.0,0.0,1}};

    int ColorsLen = sizeof(Colors)/sizeof(FLOAT_COLOR);

	SDL_Surface *screen;
	SDL_Surface *back_screen;
	SDL_Event event;

	SDL_Thread *threads[MAX_THREADS];
	THREAD_PARAM param;
    FRACTAL_PARAMETER fractal_param;

    fractal_param.x1 = -1.0;
    fractal_param.x2 = 2.0;
    fractal_param.y1 = 1.2;
    fractal_param.y2 = -1.2;

    fractal_param.max_iteration = 300;
    fractal_param.screen_xw = SCREEN_XW;
    fractal_param.screen_yw = SCREEN_YW;

    fractal_param.color_tab = NULL;

    fractal_param.color_point_tab_len = 5;
    fractal_param.color_point_tab = new FLOAT_COLOR[fractal_param.color_point_tab_len];

    int ColorsCounter = 0;
    for(int i=0; i<fractal_param.color_point_tab_len; i++)
    {
        fractal_param.color_point_tab[i] = Colors[ColorsCounter];
        ColorsCounter++;
        if(ColorsCounter == ColorsLen) ColorsCounter = 0;
    }


    cout << endl << "*** SDL Apfelmaennchen ***" << endl;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cerr << "Fehler: SDL konnte nicht initialisiert werden." << endl;
		return -1;
	}
			
	screen = SDL_SetVideoMode(SCREEN_XW,SCREEN_YW,32,SDL_HWSURFACE | SDL_DOUBLEBUF);
    back_screen = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_XW, SCREEN_YW, 32,0,0,0,0);
	SDL_WM_SetCaption("SDL Apfel", NULL);
		
    screen_ratio = (long double)screen->h / (long double)screen->w;

    param.mutex = SDL_CreateMutex();
		
    param.fractal_parameter = &fractal_param;
	StartNewFrame(&param,threads,back_screen);
	
	bool ende = false;
 	while(!ende)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				ende = true;
				break;
			case SDL_VIDEORESIZE:
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					ende = true;
					break;
				case SDLK_SPACE:
                    break;
                case SDLK_f:
                    if(!FilterIsOn)
                        FilterIsOn = true;
                    else FilterIsOn = false;
                    break;
                case SDLK_h:
                    fractal_param.max_iteration = 1500;
                    StartNewFrame(&param,threads,back_screen);
                    break;
				default:
					break;
				}
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                    if(!SelectMode)
                    {
                        SelectModeX1 = mousepos_x;
                        SelectModeY1 = mousepos_y;
                        SelectMode = true;
                    }
                    else
                    {
                        SelectMode = false;
                        SelectModeX2 = mousepos_x;
                        SelectModeY2 = mousepos_y;

                        SetNewFrame(SelectModeX1,SelectModeY1,SelectModeX2,SelectModeY1 + (mousepos_x - SelectModeX1) * screen_ratio,&fractal_param);
                        StartNewFrame(&param,threads,back_screen);
                    }
                    break;

                case SDL_BUTTON_RIGHT:
                    if(SelectMode)
                    {
                        SelectMode = false;
                    }
                    else
                    {
                        fractal_param.x1 = -1.0;
                        fractal_param.x2 = 2.0;
                        fractal_param.y1 = 1.0;
                        fractal_param.y2 = -1.0;
                        fractal_param.max_iteration = 150;
                        StartNewFrame(&param,threads,back_screen);
                    }
                    break;
                }
                break;

			case SDL_MOUSEMOTION:
				mousepos_x = event.motion.x;
				mousepos_y = event.motion.y;
				break;
			}
		}
		
        SDL_LockMutex (param.mutex);
        SDL_BlitSurface(back_screen, NULL, screen, NULL);
        SDL_UnlockMutex (param.mutex);


        if(SelectMode)
        {
            rectangleRGBA(screen,SelectModeX1,SelectModeY1,mousepos_x,SelectModeY1 + (mousepos_x - SelectModeX1) * screen_ratio,255,0,0,255);
        }

        if(param.PictureIsFinish && FilterIsOn)
        {
            param.PictureIsFinish = false;
            cout << "Filter Berechnung" << endl;

            unsigned int *buffer1 = new unsigned int[back_screen->h*back_screen->w*sizeof(unsigned int)];

            SDL_LockSurface(back_screen);

            unsigned int *buffer_dst = buffer1;
            unsigned int *buffer_src;

            int y = 0;

            while(y < back_screen->h)
            {
                buffer_src = (unsigned int*)((unsigned char*)back_screen->pixels + y * back_screen->pitch);


                for(int x=0;x<(back_screen->w);x++)
                {
                    *buffer_dst++ = *buffer_src++;
                }

                y++;
            }

            buffer_src = buffer1;
            int buffer_dst_pitch = back_screen->w;
            y = 1;
            while(y < back_screen->h-1)
            {

                //SDL_LockMutex (m);

                buffer_dst = (unsigned int*)((unsigned char*)back_screen->pixels + y * back_screen->pitch) +1;

                for(int x=1;x<(back_screen->w-1);x++)
                {
                    unsigned int r=0;
                    unsigned int g=0;
                    unsigned int b=0;

                    for(int cy=0;cy<3;cy++)
                        for(int cx=0;cx<3;cx++)
                        {
                            r += ((*(buffer_src+(cy*buffer_dst_pitch))+cx) >> 16) & 0x000000ff;
                            g += ((*(buffer_src+(cy*buffer_dst_pitch))+cx) >> 8) & 0x000000ff;
                            b += (*(buffer_src+(cy*buffer_dst_pitch))+cx) & 0x000000ff;
                        }


                    r += ((*(buffer_src+(1*buffer_dst_pitch))+1) >> 16) & 0x000000ff;
                    g += ((*(buffer_src+(1*buffer_dst_pitch))+1) >> 8) & 0x000000ff;
                    b += (*(buffer_src+(1*buffer_dst_pitch))+1) & 0x000000ff;

                    r /= 10;
                    g /= 10;
                    b /= 10;

                    *buffer_dst++ = (r&255)<<16 | (g&255)<<8 | (b&255);
                    buffer_src++;
                }

                buffer_src += 2;

                y++;
            }
            SDL_UnlockSurface(back_screen);

            delete buffer1;
        }

		SDL_Flip(screen);
        SDL_Delay(20);
	}
	
    SDL_DestroyMutex (param.mutex);
    SDL_Quit();
	
    if(fractal_param.color_tab != NULL) free(fractal_param.color_tab);

	return 0;
}

void StartNewFrame(THREAD_PARAM *param, SDL_Thread **threads , SDL_Surface *screen)
{
	param->AktZeile = 0;
	param->screen = screen;	

    param->fractal_parameter->color_tab = (unsigned int*)realloc(param->fractal_parameter->color_tab,param->fractal_parameter->max_iteration * sizeof(unsigned int));
    SetColorTab(param->fractal_parameter->color_tab, param->fractal_parameter->max_iteration,param->fractal_parameter->color_point_tab,param->fractal_parameter->color_point_tab_len);

    param->PictureIsFinish = false;

    for(int i=0; i<MAX_THREADS; i++)
        threads[i] = SDL_CreateThread (Thread1, param);
}

int Thread1 (void *p)
{	
	THREAD_PARAM *param = (THREAD_PARAM*)p;
	
	SDL_mutex *m = (SDL_mutex*) param->mutex;

	while(param->AktZeile < param->screen->h)
	{

        //SDL_LockMutex (m);
		SDL_LockSurface(param->screen);
		unsigned int*buffer = (unsigned int*)((unsigned char*)param->screen->pixels + param->AktZeile * param->screen->pitch);	
        CalcZeile(buffer,param->AktZeile,param->fractal_parameter);
		SDL_UnlockSurface(param->screen);
        //SDL_UnlockMutex (m);
		param->AktZeile++;
        //SDL_Delay(5);
	}

    if(param->AktZeile == param->screen->h)
        param->PictureIsFinish = true;
}

void CalcZeile(unsigned int *buffer, int y, FRACTAL_PARAMETER *fractal_param)
{
    long double dx,dy;

    dy = fractal_param->y1 - (((fractal_param->y1 - fractal_param->y2) / (long double)(fractal_param->screen_yw-1)) * (long double)y);

    for(int i=0; i<fractal_param->screen_xw; i++)
    {
      dx = fractal_param->x1 - (((fractal_param->x1 - fractal_param->x2) / (long double)(fractal_param->screen_xw-1)) * (long double)i);
      *buffer = CalcColor(dx,dy,fractal_param->max_iteration,fractal_param->color_tab);
      buffer++;
    }
}

unsigned int CalcColor(long double x, long double y, int max_iteration, unsigned int *color_tab)
{
    // Z und C als Komplexe Zahle definieren //
    kplx::ALG_ZAHL Z = {0,0};
    kplx::ALG_ZAHL C = {x,y};  // Realteil X-Achse -- Imaginärteil Y-Achse

    unsigned int color;
    int n = 0;

    while((kplx::ConvertToExpForm(Z).r < 2.0) && (n < max_iteration)) // wenn r >= 2 so strebt die Rekursion gegen unendlich
    {
        Z = kplx::SubAlgForm(kplx::MulAlgForm(Z,Z),C);
        n++;
    }


    if(n < max_iteration)
    {
        color = color_tab[n];
    }

    else color = 0x00000000;

    return color;
}

void SetNewFrame(int x1, int y1, int x2, int y2, FRACTAL_PARAMETER *fractal_param)
{
    long double dx1,dy1,dx2,dy2;

    dx1 = fractal_param->x1 - (((fractal_param->x1 - fractal_param->x2) / (double)(fractal_param->screen_xw-1)) * (double)x1);
    dy1 = fractal_param->y1 - (((fractal_param->y1 - fractal_param->y2) / (double)(fractal_param->screen_yw-1)) * (double)y1);
    dx2 = fractal_param->x1 - (((fractal_param->x1 - fractal_param->x2) / (double)(fractal_param->screen_xw-1)) * (double)x2);
    dy2 = fractal_param->y1 - (((fractal_param->y1 - fractal_param->y2) / (double)(fractal_param->screen_yw-1)) * (double)y2);

    fractal_param->x1 = dx1;
    fractal_param->y1 = dy1;
    fractal_param->x2 = dx2;
    fractal_param->y2 = dy2;
}

void SetColorTab(unsigned int *color_tab, int max_iteration, FLOAT_COLOR *color_point_tab, int color_point_tab_len)
{
    float step1 = 1.0 / (float) max_iteration;
    float step2 = 1.0 / (float) (color_point_tab_len);

    float counter1 =  0;
    float cmp1 = step2;
    int c1 = 0;

    float r = color_point_tab[c1].r;
    float g = color_point_tab[c1].g;
    float b = color_point_tab[c1].b;

    float r_add = (color_point_tab[c1+1].r - r)/((float)max_iteration / (float)color_point_tab_len);
    float g_add = (color_point_tab[c1+1].g - g)/((float)max_iteration / (float)color_point_tab_len);
    float b_add = (color_point_tab[c1+1].b - b)/((float)max_iteration / (float)color_point_tab_len);

    for(int i=0; i<max_iteration; i++)
    {
        color_tab[i] = (unsigned int)(r*255)<< 16 | (unsigned int)(g*255)<< 8 | (unsigned int)(b*255);

        r += r_add;
        g += g_add;
        b += b_add;

        if(r > 1.0) r = 1.0;
        if(g > 1.0) g = 1.0;
        if(b > 1.0) b = 1,0;

        if(r < 0) r = 0;
        if(g < 0) g = 0;
        if(b < 0) b = 0;

        counter1 += step1;
        if(counter1 >= cmp1)
        {
            c1++;
            cmp1 += step2;

            r_add = (color_point_tab[c1+1].r - r)/((float)max_iteration / (float)color_point_tab_len);
            g_add = (color_point_tab[c1+1].g - g)/((float)max_iteration / (float)color_point_tab_len);
            b_add = (color_point_tab[c1+1].b - b)/((float)max_iteration / (float)color_point_tab_len);
        }

    }
}
