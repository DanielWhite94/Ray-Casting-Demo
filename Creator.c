#include "Creator.h"

int main(int argc, char **argv)
{
	int TileData0[]={
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0, -1, -1, -1, -1,  2, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0, -1, -1, -1, -1, -1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	  0, -1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	  0, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	  0, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1,  4,  4, -1, -1, -1, -1,
	  0, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1,  4, -1, -1, -1, -1,
	  0,  0,  0, -1, -1,  0,  6,  0,  0,  0, -1, -1, -1, -1, -1,  4, -1, -1, -1, -1,
	  5,  5,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  4, -1, -1, -1, -1,
	  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  3, -1,  3, -1, -1, -1, -1, -1, -1,
	  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  3, -1, -1, -1, -1, -1, -1,
	  5, -1, -1, -1, -1, -1, -1,  5, -1, -1, -1,  3,  3,  3, -1, -1, -1, -1, -1, -1,
	  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	int Visible0[]={1, -1};
	SaveSection(0, 0, 0, 20, 20, Visible0, TileData0);

	int TileData1[]={
		6,  6, -1,  6,  6,
		6,  6, -1,  6,  6,
		6,  6,  6,  6,  6,
		6,  6,  6,  6,  6,
		6,  6,  6,  6,  6};
	int Visible1[]={0,-1};
	SaveSection(1, 20, 0, 25, 5, Visible1, TileData1);

	ClearTiles();	
	SaveTile(0, 0x33DD66, 512, NULL);
	SaveTile(1, 0xFF0000, 512, NULL);
	SaveTile(2, 0x00FF00, 10000, NULL);
	SaveTile(3, 0x0000FF, 1024, NULL);
	SaveTile(4, 0, 300, "Tiles/redbrick.png");
	SaveTile(5, 0, 800, "Tiles/stone.png");
	SaveTile(6, 0, 512, "Tiles/window.png");

	return 0;
}

void SaveTile(int Id, int Colour, int Height, char *Filename)
{
	FILE *File=fopen("Tiles/TileList", "ab");
	int Zero=0;
	
	fwrite(&Id, sizeof(int), 1, File);
	if (Filename==NULL)
		fwrite(&Zero, sizeof(char), 1, File);
	else
		fwrite(Filename, sizeof(char), strlen(Filename)+1, File);	
	fwrite(&Colour, sizeof(int), 1, File);
	fwrite(&Height, sizeof(int), 1, File);
	
	fclose(File);
}

void ClearTiles()
{
	FILE *File=fopen("Tiles/TileList", "wb");
	fclose(File);
}

void SaveSection(int Id, long long int X1, long long int Y1, long long int X2,
				 long long int Y2, int *Visible, int *TileData)
{
	char Filename[32];
	sprintf(Filename, "Maps/%i", Id);
	FILE *File=fopen(Filename, "wb");
	
	fwrite(&Id, sizeof(int), 1, File);
	fwrite(&X1, sizeof(long long int), 1, File);
	fwrite(&Y1, sizeof(long long int), 1, File);
	fwrite(&X2, sizeof(long long int), 1, File);
	fwrite(&Y2, sizeof(long long int), 1, File);
	int I=-1;
	do
	{
		++I;
		fwrite(&(Visible[I]), sizeof(int), 1, File);
	}while(Visible[I]!=-1);
	fwrite(TileData, sizeof(int), (X2-X1)*(Y2-Y1), File);
		
	fclose(File);
}
