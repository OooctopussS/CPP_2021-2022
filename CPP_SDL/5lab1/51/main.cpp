#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;

/* Построить график функции
y = 5x/(1-x)^2 при x [-7,0]*/

SDL_DisplayMode displayMode;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* windowSurface = NULL;
SDL_Surface* text = NULL; // Текст
TTF_Font* font = NULL; // Шрифт

class LTexture
{
private:
    SDL_Texture* mTexture;

    int mWidth;
    int mHeight;
public:

        //Initializes variables
        LTexture()
        {
            //Initialize
            mTexture = NULL;
            mWidth = 0;
            mHeight = 0;
        }

        //Deallocates memory
        ~LTexture()
        {//Deallocate
            free();
        }

        #if defined(SDL_TTF_MAJOR_VERSION)
        //Creates image from font string
        bool loadFromRenderedText(std::string textureText, SDL_Color textColor)
        {
            //Get rid of preexisting texture
            free();

            //Render text surface
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor );
            if( textSurface != NULL )
            {
                //Create texture from surface pixels
                mTexture = SDL_CreateTextureFromSurface(renderer, textSurface );
                if( mTexture == NULL )
                {
                    printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
                }
                else
                {
                    //Get image dimensions
                    mWidth = textSurface->w;
                    mHeight = textSurface->h;
                }

                //Get rid of old surface
                SDL_FreeSurface( textSurface );
            }
            else
            {
                printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
            }


            //Return success
            return mTexture != NULL;
        }
        #endif

        //Deallocates texture
        void free()
        {
            //Free texture if it exists
            if( mTexture != NULL )
            {
                SDL_DestroyTexture( mTexture );
                mTexture = NULL;
                mWidth = 0;
                mHeight = 0;
            }
        }

        //Renders texture at given point
        void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE )
        {
            //Set rendering space and render to screen
            SDL_Rect renderQuad = { x, y, mWidth, mHeight };

            //Set clip rendering dimensions
            if( clip != NULL )
            {
                renderQuad.w = clip->w;
                renderQuad.h = clip->h;
            }

            //Render to screen
            SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip );
        }

        //Gets image dimensions
        int getWidth()
        {
             return mWidth;
        }

        int getHeight()
        {
            return mHeight;
        }
};


LTexture xMinTextTexture;
LTexture xMaxTextTexture;
LTexture xMinInputTextTexture;
LTexture xMaxInputTextTexture;

bool init() {
    bool ok = true;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "Can't init SDL: " << SDL_GetError() << endl;
        ok = false;
    }

    if(SDL_GetDesktopDisplayMode(0,&displayMode) != 0){
        cout << "Can't take your display size " << SDL_GetError() << endl;;
        ok = false;
    }

    if(TTF_Init() < 0){
        cout << "Can't init TTF: " << SDL_GetError() << endl;
        ok = false;
    }

    font = TTF_OpenFont("DroidSans.ttf", 50);
    if(!font) cout << "Font Error: " << SDL_GetError() << endl;

    window = SDL_CreateWindow("Dratuti", 0, 0, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN);
    if (window == nullptr){
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        ok = false;
    }

    windowSurface = SDL_GetWindowSurface(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) {
            cout << "Can't create renderer: " << SDL_GetError() << endl;
            ok = false;
        }
    return ok;
}

void quit() {

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_FreeSurface(text);
    text = NULL;

    TTF_Quit();
    SDL_Quit();
}

void enterLimits(double &xmin, double &xmax)
{
    SDL_Event event;
    bool quit = false;
    SDL_Color textColor {255,255,255,0};
    string number = "";
    int condition = 0;

    xMinTextTexture.loadFromRenderedText("Enter xmin:", textColor );
    xMaxTextTexture.loadFromRenderedText("Enter xmin:", textColor );

    SDL_StartTextInput();
    while(!quit)
    {
        bool renderText = false;

        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                }
                if(event.key.keysym.sym == SDLK_RETURN && number.length() != 0)
                {
                    if (condition == 0)
                    {
                        xmin = atoi(number.c_str());
                        number.clear();
                        condition++;
                    }
                    else if (condition == 1)
                    {
                        xmax = atoi(number.c_str());
                        quit = true;
                    }
                }

                //Handle backspace
                if(event.key.keysym.sym == SDLK_BACKSPACE && number.length() > 0 )
                {
                    //lop off character
                    number.pop_back();
                    renderText = true;
                }
                //Handle copy
                else if(event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
                {
                    SDL_SetClipboardText(number.c_str() );
                }
                //Handle paste
                else if(event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
                {
                    number = SDL_GetClipboardText();
                    renderText = true;
                }
            }
            else if(event.type == SDL_TEXTINPUT)
            {
                if(number.length() < 10)
                {
                    //Not copy or pasting
                    if( !( SDL_GetModState() & KMOD_CTRL && ( event.text.text[ 0 ] == 'c' || event.text.text[ 0 ] == 'C' || event.text.text[ 0 ] == 'v' || event.text.text[ 0 ] == 'V' ) ) )
                    {
                        renderText = true;
                       number += event.text.text;
                    }
                }
            }

        }

        if(renderText)
        {
            //Text is not empty
            if(number != "")
            {
                //Render new text
                if(condition == 0) xMinInputTextTexture.loadFromRenderedText( number.c_str(), textColor );
                if(condition == 1) xMaxInputTextTexture.loadFromRenderedText( number.c_str(), textColor );
            }
            //Text is empty
            else
            {
                //Render space texture
             if(condition == 0) xMinInputTextTexture.loadFromRenderedText( " ", textColor );
             if(condition == 1) xMaxInputTextTexture.loadFromRenderedText( " ", textColor );
            }
        }

        //Clear screen
        SDL_SetRenderDrawColor(renderer, 27, 25, 22, 0 );
        SDL_RenderClear(renderer);

        //Render text textures
        xMinTextTexture.render((displayMode.w - xMinTextTexture.getWidth() * 4) / 2,( displayMode.h - xMinTextTexture.getHeight() * 4) / 2);
        xMinInputTextTexture.render( (displayMode.w - xMinTextTexture.getWidth()) / 2, ( displayMode.h - xMinTextTexture.getHeight() * 4) / 2);
        xMaxTextTexture.render((displayMode.w - xMaxTextTexture.getWidth() * 4) / 2,( displayMode.h - xMaxTextTexture.getHeight()) / 2);
        xMaxInputTextTexture.render( (displayMode.w - xMinTextTexture.getWidth()) / 2, ( displayMode.h - xMaxTextTexture.getHeight()) / 2);
        //Update screen
        SDL_RenderPresent(renderer);
    }
    SDL_StopTextInput();
}

void graphDraw(double &xmin, double &xmax)
{
    SDL_Event event;
    SDL_Color textColor {255,255,255,0};
    bool quit = false;
    bool graphIsDrown = false;
    bool itFirstStep = true;
    Sint16 x0scr = 0, y0scr, xscr = 0, yscr;
    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            }
        }
        if(!graphIsDrown)
        {
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(renderer);

            y0scr = displayMode.h / 2;
            yscr = y0scr;
            double  dx, x, y, mx,my;
            dx = abs(xmax - xmin) / (double)displayMode.w ;
            double k = 0;
            // Маштаб для насечек
            double notchesMX, notchesMY;
            int oldxScr, oldyScr;

            mx = displayMode.w / (xmax - xmin);
            my = mx;

            // xscr - текущая
            // yscr - текущая
            // x0scr - ось(нулевая)
            // y0scr - ось(нулевая)

            SDL_SetRenderDrawColor(renderer, 226, 88, 34, 0x00);
            for(x = xmin; x <= xmax; x += dx)
            {

                if(abs(x) < 0.00001){
                    x0scr = xscr;
                    k = 0;
                }
                if(abs(x - 1) > 0.000001)
                {
                    y = 5 * x / ((1 - x) * (1 - x));
                    xscr = x0scr + floor(k * mx);
                    yscr = y0scr - floor(y * my);
                   if (!itFirstStep){
                       if(yscr > 0 && oldyScr > 0)
                       {
                           if(abs(abs(oldyScr) - abs(yscr)) < 100)
                           {
                               SDL_RenderDrawLine(renderer, oldxScr, oldyScr, xscr,yscr);
                           }
                       }
                   }
                   else
                   {
                       itFirstStep = false;
                   }
                   oldxScr = xscr;
                   oldyScr = yscr;
                } else {
                    xscr = x0scr + floor(k * mx);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 204, 0);
                    // ASSимптота
                    SDL_RenderDrawLine(renderer, xscr, 0, xscr, displayMode.h);
                    SDL_SetRenderDrawColor(renderer, 226, 88, 34, 0x00);
                }
                k += dx;
            }
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
            // Ось x
            SDL_RenderDrawLine(renderer, 0, y0scr, displayMode.w, y0scr);
            // Ось y
           if(x0scr != 0 || xmin == 0) SDL_RenderDrawLine(renderer, x0scr, 0, x0scr, displayMode.h);
           int oneDiv = 1;
           for(double tmp = 40; tmp > mx; tmp /= 10, oneDiv *=10);
           notchesMX = notchesMY = mx * oneDiv;

           font = TTF_OpenFont("DroidSans.ttf", 10);
           // Насечки по x
           SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
           int tmpNumX = xmin;
           bool firstX = true;
           for(xscr = 0; xscr <= displayMode.w; xscr += notchesMX){
               if(!firstX) tmpNumX += oneDiv;
               string tmpString;
               tmpString = std::to_string(tmpNumX);
               xMinTextTexture.loadFromRenderedText(tmpString.c_str(), textColor);
               if(xscr == displayMode.w) xMinTextTexture.render(xscr - 12, y0scr);
               else xMinTextTexture.render(xscr, y0scr);
               SDL_RenderDrawLine(renderer,  xscr, y0scr - 4, xscr, y0scr + 4);
               firstX = false;
           }

           // Насечки по y
           if(x0scr != 0 || xmin == 0)
           {
               int tmpNumY = 0;
               bool firstY = true;
               for(yscr = displayMode.h / 2; yscr <= displayMode.h; yscr += notchesMY)
               {
                   if(!firstY) tmpNumY -= oneDiv;
                   string tmpString;
                   tmpString = std::to_string(tmpNumY);
                   xMinTextTexture.loadFromRenderedText(tmpString.c_str(), textColor);
                  if(xmax == 0) xMinTextTexture.render(x0scr - 12, yscr);
                  else xMinTextTexture.render(x0scr, yscr);
                   SDL_RenderDrawLine(renderer,  x0scr - 4, yscr, x0scr + 4, yscr);
                   firstY = false;
               }

               tmpNumY = oneDiv;
               firstY = true;
               for(yscr = displayMode.h / 2 - notchesMY; yscr >= 0; yscr -= notchesMY)
               {
                   if(!firstY) tmpNumY += oneDiv;
                   string tmpString;
                   tmpString = std::to_string(tmpNumY);
                   xMinTextTexture.loadFromRenderedText(tmpString.c_str(), textColor);
                   if(xmax == 0) xMinTextTexture.render(x0scr - 12, yscr);
                  else xMinTextTexture.render(x0scr, yscr);
                   SDL_RenderDrawLine(renderer,  x0scr - 4, yscr, x0scr + 4, yscr);
                   firstY = false;
               }
           }

           SDL_RenderPresent(renderer);
           graphIsDrown = true;
        }
    }
}

int main (int argc, char* args[])
{

    if (!init()) {
        quit();
        system("pause");
        return 1;
    }

    double xmin, xmax;

    enterLimits(xmin, xmax);
    graphDraw(xmin, xmax);


    quit();

    return 0;
}


