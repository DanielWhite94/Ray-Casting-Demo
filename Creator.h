#ifndef CREATOR_H
#define CREATOR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ClearTiles();
void SaveTile(int Id, int Colour, int Height, char *Filename);
void SaveSection(int Id, long long int X1, long long int Y1, long long int X2, long long int Y2, int *Visible, int *TileData);

#endif
