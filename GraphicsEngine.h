#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include "Level.h"

#define GE_PI 3.14159

typedef struct
{
	double X;
	double Y;
	double Angle;/* Which way the view is facing */
	double FOV;/* Field of view - 'width' (given as an angle) viewer can see */
	double MaxDistance;/* How far to search for a tile before giving up */
}_Viewpoint;

typedef enum
{
	_GEType_Sprite,
	_GEType_Tile
}_GEType;

typedef struct
{
	_GEType Type;
	int Id;
	double Distance;
	double RayX;
	double RayY;
}_GEStack;

void GE_DrawSkybox(SDL_Surface *Surface);
void GE_DrawBlocks(_Viewpoint Viewpoint, SDL_Surface *Surface);
double GE_NormaliseAngle(double Angle);
double GE_AngleY(double Angle);
double GE_AngleX(double Angle);
void GE_DrawTile(int Id, double Distance, int XOffset, int SurfaceX, SDL_Surface *Surface);
void GE_DrawSprite(int Id, double Distance, int XOffset, int SurfaceX, SDL_Surface *Surface);
void GE_DrawSolidSlice(SDL_Surface *Surface, int Colour, int X, int Y1, int Height);
int GE_AdjustColour(int Colour, double Distance);
void GE_DrawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 GE_ReadPixel(SDL_Surface *surface, int x, int y);
void GE_ScaleBlitSurfaceSlice(SDL_Surface *Input, int SliceX, int SliceY1, int SliceY2, int DesiredHeight, double Distance, SDL_Surface *Surface, int DestX, int DestY);

#endif
