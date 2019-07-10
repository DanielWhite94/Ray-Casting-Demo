#include "Level.h"

_Tile *TileList=NULL;
int TileListCount=0;
_GlobalSectionEntry *GlobalSectionList=NULL;
int GlobalSectionListSize=0;
_SectionEntry *SectionList;
int SectionListCount;

int Level_BlockBaseHeight=64;
int Level_BlockWidth=64;

void Level_Init()
{
	/* Create global section array */
	/* Do this by looping over all files in the Maps directory */
	
	struct dirent *Entry;
	DIR *Dir=opendir("Maps");
	char Filename[32];
	FILE *File;
	GlobalSectionListSize=0;
	
	while((Entry=readdir(Dir))!=NULL)
		if (Entry->d_name[0]!='.')
		{
			/* TODO: Check for null */
			GlobalSectionList=realloc(GlobalSectionList, (GlobalSectionListSize+1)*sizeof(_GlobalSectionEntry));
		
			sprintf(Filename,"Maps/%s", Entry->d_name);
			
			File=fopen(Filename, "rb");
			fread(&(GlobalSectionList[GlobalSectionListSize].Id), sizeof(int), 1, File);
			fread(&(GlobalSectionList[GlobalSectionListSize].X1), sizeof(long long), 1, File);
			fread(&(GlobalSectionList[GlobalSectionListSize].Y1), sizeof(long long), 1, File);
			fread(&(GlobalSectionList[GlobalSectionListSize].X2), sizeof(long long), 1, File);
			fread(&(GlobalSectionList[GlobalSectionListSize].Y2), sizeof(long long), 1, File);

			fclose(File);
			++GlobalSectionListSize;
		}
	
	closedir(Dir);	
	
	/* TileList */
	File=fopen("Tiles/TileList", "rb");
	char String[256], Char;
	
	int Id;
	TileListCount=0;
	TileList=NULL;
	while(fread(&Id, sizeof(int), 1, File))
	{
		/* TODO: Check for null */
		++TileListCount;
		TileList=realloc(TileList, TileListCount*sizeof(_Tile));
		
		String[0]='\0';
		do
		{
			fread(&Char, sizeof(char), 1, File);
			String[strlen(String)+1]=0;
			String[strlen(String)]=Char;
		}
		while(Char);
		if (strlen(String)>0)
			TileList[Id].Surface=IMG_Load(String);
		else
			TileList[Id].Surface=NULL;
		
		fread(&(TileList[Id].Colour), sizeof(int), 1, File);
		fread(&(TileList[Id].Height), sizeof(int), 1, File);
	}

	fclose(File);
}

void Level_Quit()
{
	/* Free all loaded sections */
	int I;
	for(I=0;I<SectionListCount;++I)
	{
		free(SectionList[I].VisibleSections);
		SectionList[I].VisibleSections=NULL;
	}
	
	free(SectionList);
	SectionList=NULL;
	SectionListCount=0;
	
	/* Free tiles */
	for(I=0;I<TileListCount;++I)
	{
		SDL_FreeSurface(TileList[I].Surface);
		TileList[I].Surface=NULL;
	}
	free(TileList);
	TileList=NULL;
	TileListCount=0;
	
	/* Free global list */
	free(GlobalSectionList);
	GlobalSectionList=NULL;
	GlobalSectionListSize=0;
}

int Level_SpriteOnSquare(long long int X, long long int Y)
{
	/* TODO: this */
	return -1;
}

int Level_TileOnSquare(long long int X, long long int Y)
{
	/* Find which section tile is in */
	int SectionId=Level_XYToSectionId(X, Y);
	if (SectionId==-1)
		return -1;

	/* Adjust X and Y to relative offset within section */
	int Index=Level_SectionIdToIndex(SectionId);
	X-=SectionList[Index].X1;
	Y-=SectionList[Index].Y1;
	
	/* Return the tile */	
	return SectionList[Index].Tiles[X][Y];
}

_Tile Level_GetTileById(int TId)
{
	return TileList[TId];
}

void Level_UpdatePlayerPosition(double PlayerX, double PlayerY)
{
	/* Find which section the player is in */
	int SectionId=Level_XYToSectionId((int)PlayerX, (int)PlayerY);
	if (SectionId==-1)
	{
		printf("Error: No section associated with (%f, %f).\n", PlayerX, PlayerY);
		exit(0);
	}
	
	/* Firstly load the section itself */
	Level_LoadSection(SectionId);
	
	/* Set all sections as uneeded expect current section and ones visible */
	int SectionIndex=Level_SectionIdToIndex(SectionId);
	int I, J;
	for(I=0;I<SectionListCount;++I)
	{
		if (I==SectionIndex)
			continue;
	
		SectionList[I].Needed=0;
	
		for(J=0;SectionList[SectionIndex].VisibleSections[J]!=-1;++J)
			if (SectionList[I].Id==SectionList[SectionIndex].VisibleSections[J])
				SectionList[I].Needed=1;
	}
	
	/* Now load all sections which are visible from this one */
	for(I=0;SectionList[SectionIndex].VisibleSections[I]!=-1;++I)
		Level_LoadSection(SectionList[SectionIndex].VisibleSections[I]);
}

int Level_XYToSectionId(long long int X, long long int Y)
{
	int I;
	for(I=0;I<GlobalSectionListSize;++I)
		if (X>=GlobalSectionList[I].X1 && Y>=GlobalSectionList[I].Y1 &&
			X<GlobalSectionList[I].X2 && Y<GlobalSectionList[I].Y2)
			return GlobalSectionList[I].Id;
			
	return -1;
}

void Level_LoadSection(int SectionId)
{
	/* Is struct already loaded? */
	if (Level_SectionIdToIndex(SectionId)!=-1)
		/* No point loading it again */
		return;
		
	/* Find somewhere to store the new section */
	int Index, I;
	for(Index=0;Index<SectionListCount;++Index)
		if (!SectionList[Index].Needed)
		{
			/* Clean up malloc'ed stuff */
			free(SectionList[Index].VisibleSections);
			SectionList[Index].VisibleSections=NULL;
			for(I=0;I<SectionList[Index].Height;++I)
				free(SectionList[Index].Tiles[I]);
			free(SectionList[Index].Tiles);
			SectionList[Index].Tiles=NULL;
			
			break;
		}
	
	if (Index==SectionListCount)
	{
		/* No space - must allocate more */
		/* TODO: Check for nulls */
		Index=SectionListCount++;
		SectionList=realloc(SectionList, SectionListCount*sizeof(_SectionEntry));
	}
	
	/* Load the section from its file */
	char Filename[32];
	sprintf(Filename, "Maps/%i", SectionId);
	FILE *File=fopen(Filename, "rb");
	fread(&(SectionList[Index].Id), sizeof(int), 1, File);
	fread(&(SectionList[Index].X1), sizeof(long long), 1, File);
	fread(&(SectionList[Index].Y1), sizeof(long long), 1, File);
	fread(&(SectionList[Index].X2), sizeof(long long), 1, File);
	fread(&(SectionList[Index].Y2), sizeof(long long), 1, File);
	SectionList[Index].Width=SectionList[Index].X2-SectionList[Index].X1;
	SectionList[Index].Height=SectionList[Index].Y2-SectionList[Index].Y1;

	I=-1;
	SectionList[Index].VisibleSections=NULL;
	do
	{
		++I;
		SectionList[Index].VisibleSections=realloc(SectionList[Index].VisibleSections, (I+1)*sizeof(int));
		fread(&(SectionList[Index].VisibleSections[I]), sizeof(int), 1, File);
	}while(SectionList[Index].VisibleSections[I]!=-1);
	
	SectionList[Index].Tiles=malloc(sizeof(int*)*SectionList[Index].Height);
	for(I=0;I<SectionList[Index].Height;++I)
	{
		SectionList[Index].Tiles[I]=malloc(sizeof(int)*SectionList[Index].Width);
		fread(SectionList[Index].Tiles[I], sizeof(int), SectionList[Index].Width, File);
	}
	
	SectionList[Index].Needed=1;
	fclose(File);
}

int Level_SectionIdToIndex(int SectionId)
{
	int I;
	for(I=0;I<SectionListCount;++I)
		if (SectionList[I].Id==SectionId)
			return I;
	
	return -1;
}
