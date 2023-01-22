#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <time.h>

using namespace std;

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
int SCREEN_CENTER_WIDTH = SCREEN_WIDTH / 2;

SDL_Window *window = NULL;
SDL_Renderer *render = NULL;
SDL_Texture *image1 = NULL;
SDL_Texture *image2 = NULL;


bool init() {
    bool ok = true;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "Can't init SDL: " << SDL_GetError() << endl;
    }

    int flags = IMG_INIT_PNG;
    if(!(IMG_Init(flags) & flags)){
       cout << "Can't init image: " << IMG_GetError() << endl;
       ok = false;
    }

    window = SDL_CreateWindow("Салют", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            cout << "Can't create window: " << SDL_GetError() << endl;
            ok = false;
        }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (render == NULL) {
            cout << "Can't create renderer: " << SDL_GetError() << endl;
            ok = false;
        }
    return ok;
}


bool load(const char* file, SDL_Texture **image) {
    bool ok = true;
    SDL_Surface * temp_surf = NULL;
    temp_surf = IMG_Load(file);
    if (temp_surf == NULL) {
        cout << "Can't load image: " << IMG_GetError() << endl;
        ok = false;
    }

    *image = SDL_CreateTextureFromSurface(render, temp_surf);
    if (image == NULL) {
        cout << "Can't create texture from surface: " << SDL_GetError() << endl;
        ok = false;
    }

    SDL_FreeSurface(temp_surf);

    return ok;
}

void Draw_MoveObject(int frame){
    int positionIskrX = SCREEN_CENTER_WIDTH - 30;
    int positionIskrY = frame + 20;
    SDL_Rect rect = {SCREEN_CENTER_WIDTH, frame, 20, 50};
    SDL_RenderCopy(render, image1, NULL, &rect);
    SDL_Rect iskr = {positionIskrX, positionIskrY, 70, 70};
    SDL_RenderCopy(render, image2, NULL, &iskr);
    SDL_RenderPresent(render);
}

void Draw_MoveAfterExplosion(int frame, double x){
    double mx=10.0, my=10.0, y;
    Sint16 x0scr, y0scr, xscr, yscr;
    x0scr=SCREEN_CENTER_WIDTH;
    y0scr=frame - 50;
    SDL_SetRenderDrawColor(render, 226, 88, 34, 0x00);
    y = (-0.01) * x * x;
    for (int i = 0; i <= 6; i++){
    xscr = x0scr+floor(x*mx) - i * 5;
    yscr=y0scr-floor(y*my) + i * 5;
    if (i % 2 == 0) SDL_RenderDrawPoint(render, xscr, yscr);
    }
}

void quit() {
    SDL_DestroyTexture(image1);
    image1 = NULL;

    SDL_DestroyTexture(image2);
    image2 = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_DestroyRenderer(render);
    render = NULL;

    SDL_Quit();
    IMG_Quit();
}

int main (int argc, char* args[]) {
    SDL_Event event;
    int framecnt = SCREEN_HEIGHT; // Движение начинается снизу окна
    Uint32 before_next_frame = 10; /* задержка между перерисовкой
    кадров 40 миллисекунд */
    bool isMove = false; // Объект находится в движении
    bool isExplosion = true; // Момент взрыва салюта
    bool isAfterExplosion = false;
  //  double x = 0; // Координата x
    // Флаг выхода
    bool isWork = true;
    if (!init()) {
        quit();
        system("pause");
        return 1;
    }
    while (isWork)
    {
        // Загружаем первое изображение
        if (!load("salute.png", &image1)) {
             quit();
             return 1;
           }
        // Загружаем второе изображение
        if (!load("iskr.png", &image2)) {
             quit();
             return 1;
           }
        // Обработка событий
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {

                isWork = false;
            }

            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                isWork = false;
            }
        }

        // Имитация движения салюта
        if(framecnt >= 200 && !isAfterExplosion){
            // Создаем начальное положение
            if(!isMove){
            SDL_Rect rect1 = {SCREEN_CENTER_WIDTH, SCREEN_HEIGHT, 20, 50};
            SDL_RenderCopy(render, image1, NULL, &rect1);
            SDL_RenderPresent(render);
            isMove = true;
            }
            SDL_RenderPresent(render);
            SDL_Delay(before_next_frame);
            SDL_SetRenderDrawColor(render, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(render);
            Draw_MoveObject(framecnt--);
        }
        else{
            if(isExplosion){
                SDL_RenderClear(render);
                load("explosion.png",&image1);
                SDL_Rect explosion = {SCREEN_CENTER_WIDTH - 80, framecnt - 200, 200, 200};
                SDL_RenderCopy(render, image1, NULL, &explosion);
                SDL_RenderPresent(render);
                SDL_Delay(1000);
                isExplosion = false;
                isAfterExplosion = true;
            }
            // После взрвыа
            SDL_RenderPresent(render);
            SDL_Delay(before_next_frame);
            SDL_SetRenderDrawColor(render, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(render);
            // Делаем рестарт
            framecnt = SCREEN_HEIGHT;
            isMove = false;
            isExplosion = true;
            isAfterExplosion = false;
         //   Draw_MoveAfterExplosion(framecnt++, x -= 0.01);
        }


        /*if(framecnt == SCREEN_HEIGHT){
            isMove = false;
            isExplosion = true;
            isAfterExplosion = false;
            x = 0;
        }*/

    }
    quit();

    return 0;
}


