/* TODO: Allow different surface for the 4 different sides of a block */
/* TODO: Consider allowing floating objects? shouldn't be too difficult */
/* TODO: Only draw what is needed when more than one tile in the stack (only draw extra bit above tiles infront)

	Problem: partially transparent tiles infront still need all of what is behind drawing */
/* TODO: Different brightness levels at different points? */
/* TODO: Effects: Fog - converge colours to white not black */
#include "GraphicsEngine.h"

void GE_DrawSkybox(SDL_Surface *Surface)
{
	/* TODO: this */
	SDL_Rect Rectangle;
	Rectangle.x=0;
	Rectangle.y=0;
	Rectangle.w=Surface->w;
	Rectangle.h=Surface->h;
	SDL_FillRect(Surface, &Rectangle, 0x000000);
}

void GE_DrawBlocks(_Viewpoint Viewpoint, SDL_Surface *Surface)
{
	double DistanceToCamera, AngleDisplacement, TrueAngle, Step, DeltaX, DeltaY;
	double DeltaLength, RayX, RayY, RayLength, Dummy;
	double XTime, YTime, OffsetA, OffsetB;
	int SurfaceX, StackIndex, SpriteId, TileId, XOffset;
	_GEStack Stack[5];
	int MaxStack=5;
	
	Step=0.02;
	
	/* Distance between viewpoint and the surface */
	DistanceToCamera=(Surface->w/2)/tan(Viewpoint.FOV/2);

	/* Loop over all columns */
	for(SurfaceX=0;SurfaceX<Surface->w;++SurfaceX)
	{
		/* Angle from centre column to this column (in the range -fov/2,fov/2) */
		AngleDisplacement=atan2(SurfaceX-Surface->w/2, DistanceToCamera);
		
		/* True angle */
		TrueAngle=GE_NormaliseAngle(Viewpoint.Angle+AngleDisplacement);
		
		/* Cast a ray */
		DeltaX=GE_AngleX(TrueAngle)*Step;
		DeltaY=GE_AngleY(TrueAngle)*Step;
		DeltaLength=sqrt(DeltaX*DeltaX+DeltaY*DeltaY);
		RayX=Viewpoint.X;
		RayY=Viewpoint.Y;
		RayLength=0;
		StackIndex=0;
		
		do
		{
			/* Jump to next square */
			if (DeltaX<0)
				XTime=(ceil(RayX-1)-RayX)/DeltaX;
			else
				XTime=(floor(RayX+1)-RayX)/DeltaX;
			if (DeltaY<0)
				YTime=(ceil(RayY-1)-RayY)/DeltaY;
			else
				YTime=(floor(RayY+1)-RayY)/DeltaY;
			
			if (YTime<XTime)
				XTime=YTime;
			
			XTime+=0.001;
			RayX+=DeltaX*XTime;
			RayY+=DeltaY*XTime;
			RayLength+=DeltaLength*XTime;
			
			/* Intersection with a 2D sprite? */
			if (StackIndex+1<MaxStack)
			{
				SpriteId=Level_SpriteOnSquare((int)RayX, (int)RayY);
				if (SpriteId!=-1)
				{
					/* Add to stack */
					Stack[StackIndex].Type=_GEType_Sprite;
					Stack[StackIndex].Id=SpriteId;
					Stack[StackIndex].Distance=fabs(RayLength*cos(AngleDisplacement));
					Stack[StackIndex].RayX=RayX;
					Stack[StackIndex].RayY=RayY;
					++StackIndex;
				}
			}
			
			/* Intersection with tile */
			TileId=Level_TileOnSquare((int)RayX, (int)RayY);
			if (TileId!=-1)
			{
				/* Add to stack */
				Stack[StackIndex].Type=_GEType_Tile;
				Stack[StackIndex].Id=TileId;
				Stack[StackIndex].Distance=fabs(RayLength*cos(AngleDisplacement));
				Stack[StackIndex].RayX=RayX;
				Stack[StackIndex].RayY=RayY;
				++StackIndex;
			}
		}while(RayLength<Viewpoint.MaxDistance && StackIndex<MaxStack);
		
		/* Draw everything on the stack in reverse order */
		for(--StackIndex;StackIndex>=0;--StackIndex)
		{
			OffsetA=fabs(modf(Stack[StackIndex].RayX, &Dummy));
			OffsetB=fabs(modf(Stack[StackIndex].RayY, &Dummy));
			if (fabs(OffsetA-0.5)>fabs(OffsetB-0.5))
				XOffset=OffsetB*(double)Level_BlockWidth;
			else
				XOffset=OffsetA*(double)Level_BlockWidth;
		
			switch(Stack[StackIndex].Type)
			{
				case _GEType_Tile:
					GE_DrawTile(Stack[StackIndex].Id, Stack[StackIndex].Distance,
								XOffset, SurfaceX, Surface);
				break;
				case _GEType_Sprite:
					GE_DrawSprite(Stack[StackIndex].Id, Stack[StackIndex].Distance,
								XOffset, SurfaceX, Surface);
				break;
			}
		}
	}
}

double GE_NormaliseAngle(double Angle)
{
	/* TODO: Fast version of this */
	while(Angle<=-GE_PI)
		Angle+=2*GE_PI;
	while(Angle>GE_PI)
		Angle-=2*GE_PI;
	
	return Angle;
}

double GE_AngleY(double Angle)
{
	if(Angle<GE_PI/2)
		return -cos(Angle);
	if(Angle<GE_PI)
		return sin(Angle-GE_PI/2);
	if(Angle<(3*GE_PI)/2)
		return cos(Angle-GE_PI);
	return -sin(Angle-(3*GE_PI)/2);
}

double GE_AngleX(double Angle)
{
	if(Angle<GE_PI/2)
		return sin(Angle);
	if(Angle<GE_PI)
		return cos(Angle-GE_PI/2);
	if(Angle<(3*GE_PI)/2)
		return -sin(Angle-GE_PI);
	return -cos(Angle-(3*GE_PI)/2);
}

void GE_DrawTile(int Id, double Distance, int XOffset, int SurfaceX, SDL_Surface *Surface)
{
	_Tile Tile=Level_GetTileById(Id);
	
	/* Find how high the slice will appear on the surface */
	int Height=((double)Tile.Height)/Distance;
	
	int BaseHeight=((double)Level_BlockBaseHeight*8)/Distance;
	int TopY=(Surface->h-BaseHeight)/2-Height+BaseHeight;/* How far down the 
											  surface the slice will be drawn */
											  
	/* Do we even need to draw anything? */
	if (TopY>=Surface->h || TopY+Height<0)
		return;
											  
	/* If tile is a solid colour the rest can be skipped */
	if (Tile.Surface==NULL)
	{
		/* As this is only a solid colour we can 'crop' it to the boundary of
		   the screen */
		if (Height>Surface->h-TopY)
		Height=Surface->h-TopY;
		if (TopY<0)
		{
			Height+=TopY;
			TopY=0;
		}
		
		GE_DrawSolidSlice(Surface, GE_AdjustColour(Tile.Colour, Distance), SurfaceX, TopY, Height);
		return;
	}
	
	/* Only scale and draw part of the texture image we need */
	int SliceY1;
	int SliceY2;

	if (TopY+Height<Surface->h && TopY>=0)
	{
		/* Full surface needed */
		SliceY1=0;
		SliceY2=Tile.Surface->h;
	}
	else if (TopY+Height>=Surface->h && TopY>=0)
	{
		/* Top of the surface is needed, but we can crop some of the bottom off */
		SliceY1=0;
		SliceY2=((Surface->h-TopY)*Tile.Surface->h)/Height;
		Height=Surface->h-TopY;
	}
	else if (TopY+Height<Surface->h && TopY<0)
	{
		/* TODO: this */
		SliceY1=0;
		SliceY2=Tile.Surface->h;
	}
	else /* TopY+Height>=Surface->h && TopY<0 */
	{
		/* TODO: this */
		SliceY1=0;
		SliceY2=Tile.Surface->h;
	}
	
	GE_ScaleBlitSurfaceSlice(Tile.Surface, XOffset, SliceY1, SliceY2, Height, Distance, Surface, SurfaceX, TopY);
}

void GE_DrawSprite(int Id, double Distance, int XOffset, int SurfaceX, SDL_Surface *Surface)
{
	/* TODO: this */
}

void GE_DrawSolidSlice(SDL_Surface *Surface, int Colour, int X, int Y1, int Height)
{
	SDL_Rect Rectangle;
	Rectangle.x=X;
	Rectangle.y=Y1;
	Rectangle.w=1;
	Rectangle.h=Height;
	SDL_FillRect(Surface, &Rectangle, Colour);
}

/* Adjust the colour depending on how far away it is */
int GE_AdjustColour(int Colour, double Distance)
{
	int A=Colour>>24;
	int R=(Colour>>16) & 255;
	int G=(Colour>>8) & 255;
	int B=Colour & 255;
	
	/* Objects further away should appear darker */
	Distance=sqrt(Distance)/2;
	
//	switch(EFFECT)
//	{
//		case NORMAL:
			R/=Distance;
			G/=Distance;
			B/=Distance;
//		break;
//		case FOG:
//			R=255-(255-R)/Distance;
//			G=255-(255-G)/Distance;
//			B=255-(255-B)/Distance;
//		break;
//	}
	if (R>255)
		R=255;
	else if (R<0)
		R=0;
	if (G>255)
		G=255;
	else if (G<0)
		G=0;
	if (B>255)
		B=255;
	else if (B<0)
		B=0;
	
	return (A<<24) | (R<<16) | (G<<8) | B;
}

void GE_DrawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

Uint32 GE_ReadPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/* TODO: Much better algorithm */
void GE_ScaleBlitSurfaceSlice(SDL_Surface *Input, int SliceX, int SliceY1, int SliceY2, int DesiredHeight, double Distance, SDL_Surface *Surface, int DestX, int DestY)
{
	/* TODO: Avoid using temp surface - or at least only init once? */
	SDL_Surface *TempSurface=SDL_CreateRGBSurface(Input->flags, 1, DesiredHeight, Input->format->BitsPerPixel, Input->format->Rmask, Input->format->Gmask, Input->format->Bmask, Input->format->Amask);

	int NewY, OldY, Colour;
	int InputHeight=SliceY2-SliceY1;

	for(NewY=0;NewY<DesiredHeight;++NewY)
	{
		OldY=(NewY*InputHeight)/DesiredHeight+SliceY1;
		Colour=GE_ReadPixel(Input, SliceX, OldY);
		Colour=GE_AdjustColour(Colour, Distance);
		GE_DrawPixel(TempSurface, 0, NewY, Colour);
	}

	SDL_Rect From;
	From.x=0;
	From.y=0;
	From.w=1;
	From.h=DesiredHeight;

	SDL_Rect To;
	To.x=DestX;
	To.y=DestY;	
	To.w=1;
	To.h=DesiredHeight;
	
	SDL_BlitSurface(TempSurface, &From, Surface, &To);
	SDL_FreeSurface(TempSurface);
}
