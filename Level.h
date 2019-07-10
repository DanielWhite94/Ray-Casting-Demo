#ifndef LEVEL_H
#define LEVEL_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

typedef struct
{
	SDL_Surface *Surface;
	int Colour;/* Only used if Surface is NULL */
	int Height;/* Height the tile is in the imaginary world */
}_Tile;

extern _Tile *TileList;
extern int TileListCount;

typedef struct
{
	long long X1;
	long long X2;
	long long Y1;
	long long Y2;
	int Id;
}_GlobalSectionEntry;

extern _GlobalSectionEntry *GlobalSectionList;
extern int GlobalSectionListSize;

typedef struct
{
	int Id;
	long long X1;
	long long X2;
	long long Y1;
	long long Y2;
	long long Width;
	long long Height;
	int Needed;
	int *VisibleSections;/* Array of section ids of sections visible from this
							section. Terminated with a -1 */
	int **Tiles;
}_SectionEntry;

extern _SectionEntry *SectionList;
extern int SectionListCount;

extern int Level_BlockBaseHeight;
extern int Level_BlockWidth;

int Level_SpriteOnSquare(long long int X, long long int Y);
int Level_TileOnSquare(long long int X, long long int Y);
_Tile Level_GetTileById(int TId);
void Level_UpdatePlayerPosition(double PlayerX, double PlayerY);
void Level_LoadSection(int SectionId);
int Level_SectionIdToIndex(int SectionId);
void Level_Init();
void Level_Quit();
int Level_XYToSectionId(long long int X, long long int Y);

#endif
