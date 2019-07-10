#include "main.h"

const float PlayerSpeed=0.10;
const float PlayerTurnSpeed=0.04;
const float FPSTarget=120.0;

SDL_Surface *Screen;

int main(int argc, char **argv)
{
  if (SDL_Init(SDL_INIT_VIDEO)==-1)
  {
    printf("Could not initialise SDL\n");
    exit(1);
  }
  Screen=SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
  //Screen=SDL_SetVideoMode(0, 0, 32, SDL_FULLSCREEN|SDL_HWSURFACE|SDL_DOUBLEBUF);
  if (Screen==NULL)
  {
    printf("Could not initialise video memory\n");
    exit(1);
  }
  printf("Screen %ix%i\n", (int)Screen->w, (int)Screen->h);

  Level_Init();

  _Viewpoint VP;
  VP.X=5;
  VP.Y=5;
  VP.Angle=DEGTORAD(0);
  VP.FOV=DEGTORAD(60);
  VP.MaxDistance=100;

  VP.X=14.874200;
  VP.Y=3.721266;
  VP.Angle=4.800000;

  while(1)
  {
    SDL_Event Event;
    while(SDL_PollEvent(&Event))
    switch(Event.type)
    {
      case SDL_QUIT:
        Level_Quit();
        SDL_FreeSurface(Screen);
        SDL_Quit();
        exit(1);
      break;
      case SDL_KEYDOWN:
        switch(Event.key.keysym.sym)
        {
          case 'q':
            if (Event.key.keysym.mod & KMOD_CTRL)
            {
              Level_Quit();
              SDL_FreeSurface(Screen);
              SDL_Quit();
              exit(1);
            }
          break;
          default:
          break;
        }
      break;
    }

    uint8_t *KeyState;
    KeyState = SDL_GetKeyState(NULL);

    if(KeyState[SDLK_UP])
    {
      VP.X+=PlayerSpeed*GE_AngleX(VP.Angle);
      VP.Y+=PlayerSpeed*GE_AngleY(VP.Angle);
    }
    if(KeyState[SDLK_DOWN])
    {
      VP.X-=PlayerSpeed*GE_AngleX(VP.Angle);
      VP.Y-=PlayerSpeed*GE_AngleY(VP.Angle);
    }
    if(KeyState[SDLK_LEFT])
    {
      VP.Angle-=PlayerTurnSpeed;
      if (VP.Angle<0)
        VP.Angle+=3.14159*2;
    }
    if(KeyState[SDLK_RIGHT])
    {
      VP.Angle+=PlayerTurnSpeed;
      if (VP.Angle>=3.14159*2)
        VP.Angle-=3.14159*2;
    }

    Level_UpdatePlayerPosition(VP.X, VP.Y);

    Uint32 Time=SDL_GetTicks();

    GE_DrawSkybox(Screen);
    GE_DrawBlocks(VP, Screen);

    Time=SDL_GetTicks()-Time;

    SDL_Flip(Screen);

    if (Time!=0)
    {
      float FPS=1000.0/((float) Time);
      printf("render time: %u fps: %4.2f\n", (unsigned int) Time, FPS);
      float TimeTarget=1000.0/FPSTarget;
      float Delay=TimeTarget-Time;
      if (Delay>0)
        SDL_Delay(Delay);
    }
    else
      printf("time: 0\n");
  }

  return 0;
}
