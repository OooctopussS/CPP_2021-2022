#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <Windows.h>
#include <list>
#include <iterator>
#include <string>

#define N 8 // 8x8 доска

using namespace std;

typedef struct movesForRecord
{
    int oldX;
    int oldY;
    int newX;
    int newY;
    bool figureWasEated;
    bool itWasCastle;
}moves;

class gameMenu;
class gameStart;
class Board;
class figure;

class LTexture
{
    public:
        LTexture()
        {
            mTexture = NULL;
            mWidth = 0;
            mHeight = 0;
        }

        ~LTexture()
        {
            free();
        }

        #if defined(SDL_TTF_MAJOR_VERSION)
        //Creates image from font string
        bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
        #endif

        void free()
        {
            if( mTexture != NULL )
            {
                SDL_DestroyTexture( mTexture );
                mTexture = NULL;
                mWidth = 0;
                mHeight = 0;
            }
        }

        //Renders texture at given point
        void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        //Gets image dimensions
        int getWidth()
        {
            return mWidth;
        }

        int getHeight()
        {
             return mHeight;
        }

    private:
        //The actual hardware texture
        SDL_Texture* mTexture;

        //Image dimensions
        int mWidth;
        int mHeight;
};

class sdl_handler
{
private:
sdl_handler() {}
~sdl_handler() {}
sdl_handler(const sdl_handler&){};
sdl_handler& operator=(sdl_handler&);

public:
int mouseX = 0;
int mouseY = 0;

SDL_DisplayMode displayMode;
SDL_Window* window = NULL;
SDL_Renderer* render = NULL;
SDL_Texture *image = NULL;
TTF_Font* font = NULL;
SDL_Surface* windowSurface = NULL;
bool isReset = false;
bool isBackToMenu = false;
string nicknName = "";
LTexture gPromptTextTexture;
LTexture gInputTextTexture;

static sdl_handler& get_instance()
{
    static sdl_handler instance;
    return instance;
}

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

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        cout << "Can't init SDL Image " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow("Chess", 0, 0, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN);
    if (window == nullptr){
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        ok = false;
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (render == NULL) {
            cout << "Can't create renderer: " << SDL_GetError() << endl;
            ok = false;
        }

    if(TTF_Init() < 0){
       cout << "Can't init TTF: " << SDL_GetError() << endl;
       ok = false;
       }

    font = TTF_OpenFont("DroidSans.ttf", 100); // RodchenkoBTT.ttf
    if(!font) cout << "Font Error: " << SDL_GetError() << endl;

    windowSurface = SDL_GetWindowSurface(window);

    return ok;
}

void quit() {
    sdl_handler::get_instance().gPromptTextTexture.free();
    sdl_handler::get_instance().gInputTextTexture.free();

    SDL_DestroyTexture(image);
    image = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_DestroyRenderer(render);
    render = NULL;

    SDL_FreeSurface(windowSurface);
    windowSurface = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
};



#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(sdl_handler::get_instance().font, textureText.c_str(), textColor );
    if( textSurface != NULL )
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(sdl_handler::get_instance().render, textSurface );
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


void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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
    SDL_RenderCopyEx(sdl_handler::get_instance().render, mTexture, clip, &renderQuad, angle, center, flip );
}

class Menus
{
public:
    virtual int show() = 0;
    virtual ~Menus(){};
};

class secondaryMenu: public Menus
{
private:
    const int numItems;
    char** labels;
    SDL_Color defaultColor;
    SDL_Surface** Display;
    SDL_Rect* pos;
    TTF_Font* fontItem;
public:
    secondaryMenu(const int num, char* lab[], SDL_Color defColor, TTF_Font* f,const int &startPosX,const int &startPosY) : numItems(num), defaultColor(defColor), fontItem(f)
    {
        labels = new char*[numItems];
        for(int i = 0; i < numItems; i++)
        {
            labels[i] = lab[i];
        }

        Display = new SDL_Surface*[numItems];

        for(int i = 0; i < numItems; i++)
        {
            Display[i] = TTF_RenderText_Solid(fontItem,labels[i],defaultColor);
        }

        pos = new SDL_Rect[numItems];
        for(int i = 0; i < numItems; i++)
        {
            int sizeObjX = Display[i]->clip_rect.w/2, sizeObjY;;
            if(i == 0) sizeObjY = - Display[i]->clip_rect.h;
            else sizeObjY = + Display[i]->clip_rect.h;
            pos[i].x = startPosX * (sdl_handler::get_instance().windowSurface->clip_rect.w / 2 - sizeObjX);
            if(i == 0) pos[i].y = startPosY * (sdl_handler::get_instance().windowSurface->clip_rect.h / 2  + sizeObjY * 2);
            else pos[i].y = pos[i-1].y + sizeObjY;
        }
    }

    int show()
    {
        SDL_FillRect(sdl_handler::get_instance().windowSurface,&sdl_handler::get_instance().windowSurface->clip_rect,SDL_MapRGB(sdl_handler::get_instance().windowSurface->format, 27,25,22));

        SDL_Event event;
        bool quit = false;
        while(!quit)
        {
            SDL_UpdateWindowSurface(sdl_handler::get_instance().window);
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_QUIT:
                        quit = true;
                        break;

                    case SDL_KEYDOWN:
                        quit = true;
                        break;

                    case  SDL_MOUSEBUTTONDOWN:
                        quit = true;
                        break;

                }
            }


            for(int i = 0; i < numItems; i += 1)
            {
                SDL_BlitSurface(Display[i],NULL,sdl_handler::get_instance().windowSurface,&pos[i]);
            }

        }
        return 1;
    }

    ~secondaryMenu()
    {
        for (int i = 0; i < numItems; i++)
        {
            SDL_FreeSurface(Display[i]);
        }
        delete[] labels;
        delete[] Display;
        delete[] pos;
    }
};

class enterNickName
{
private:
    SDL_Color textColor;
    SDL_Event event;
public:
    enterNickName(SDL_Color color): textColor(color){}
    void enter()
    {
       //SDL_Event event;
        bool quit = false;
       // SDL_Color textColor {255,255,255,0};

        sdl_handler::get_instance().gPromptTextTexture.loadFromRenderedText( "Enter a NickName:", textColor );

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
                    if(event.key.keysym.sym == SDLK_RETURN && sdl_handler::get_instance().nicknName.length() != 0)
                    {
                        quit = true;
                    }

                    //Handle backspace
                    if(event.key.keysym.sym == SDLK_BACKSPACE && sdl_handler::get_instance().nicknName.length() > 0 )
                    {
                        //lop off character
                        sdl_handler::get_instance().nicknName.pop_back();
                        renderText = true;
                    }
                    //Handle copy
                    else if(event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
                    {
                        SDL_SetClipboardText(sdl_handler::get_instance().nicknName.c_str() );
                    }
                    //Handle paste
                    else if(event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
                    {
                        sdl_handler::get_instance().nicknName = SDL_GetClipboardText();
                        renderText = true;
                    }
                }
                else if(event.type == SDL_TEXTINPUT)
                {
                    if(sdl_handler::get_instance().nicknName.length() < 10)
                    {
                        //Not copy or pasting
                        if( !( SDL_GetModState() & KMOD_CTRL && ( event.text.text[ 0 ] == 'c' || event.text.text[ 0 ] == 'C' || event.text.text[ 0 ] == 'v' || event.text.text[ 0 ] == 'V' ) ) )
                        {
                            renderText = true;
                            sdl_handler::get_instance().nicknName += event.text.text;
                        }
                    }
                }

            }

            if(renderText)
            {
                //Text is not empty
                if(sdl_handler::get_instance().nicknName != "")
                {
                    //Render new text
                    sdl_handler::get_instance().gInputTextTexture.loadFromRenderedText( sdl_handler::get_instance().nicknName.c_str(), textColor );
                }
                //Text is empty
                else
                {
                    //Render space texture
                   sdl_handler::get_instance(). gInputTextTexture.loadFromRenderedText( " ", textColor );
                }
            }

            //Clear screen
            SDL_SetRenderDrawColor(sdl_handler::get_instance().render, 27, 25, 22, 0 );
            SDL_RenderClear(sdl_handler::get_instance().render);

            //Render text textures
            sdl_handler::get_instance().gPromptTextTexture.render((sdl_handler::get_instance().displayMode.w - sdl_handler::get_instance().gPromptTextTexture.getWidth()) / 2,( sdl_handler::get_instance().displayMode.h - sdl_handler::get_instance().gPromptTextTexture.getHeight() * 4) / 2);
            sdl_handler::get_instance().gInputTextTexture.render( ( sdl_handler::get_instance().displayMode.w - sdl_handler::get_instance().gInputTextTexture.getWidth() ) / 2, ( sdl_handler::get_instance().displayMode.h - sdl_handler::get_instance().gPromptTextTexture.getHeight() * 2) / 2);

            //Update screen
            SDL_RenderPresent(sdl_handler::get_instance().render);
        }
        SDL_StopTextInput();
    }
};

class gameMenu: public Menus
{
protected:
    const int numItems; // Кол-вол пунктов меню
    char** labels; // Название пунктов меню
    SDL_Color defaultColor;
    SDL_Color hoverColor;
    SDL_Surface** menuDisplay;
    bool* selected;
    SDL_Rect* pos;
public:
    gameMenu(const int numMenu, char* lab[], SDL_Color defColor, SDL_Color hover): numItems(numMenu), defaultColor(defColor), hoverColor(hover)
    {
        labels = new char*[numItems];
        for(int i = 0; i < numItems; i++)
        {
            labels[i] = lab[i];
        }

        menuDisplay = new SDL_Surface*[numItems+1];
        for(int i = 0; i < numItems; i++)
        {
          menuDisplay[i] = TTF_RenderText_Solid(sdl_handler::get_instance().font,labels[i],defaultColor);
        }


        selected = new bool[numItems];
        for(int i = 0; i < numItems; i++)
        {
            selected[i] = false;
        }


        pos = new SDL_Rect[numItems + 1];
        for(int j = 0; j < numItems; j++)
        {
            int sizeObjX = menuDisplay[j]->clip_rect.w/2, sizeObjY;
            if(j == 0) sizeObjY = - menuDisplay[j]->clip_rect.h;
            else sizeObjY = + menuDisplay[j]->clip_rect.h;
            pos[j].x = sdl_handler::get_instance().windowSurface->clip_rect.w / 2 - sizeObjX;
            if(j == 0) pos[j].y = sdl_handler::get_instance().windowSurface->clip_rect.h / 2  + sizeObjY * 2;
            else pos[j].y = pos[j-1].y + sizeObjY;
        }
        menuDisplay[numItems] = TTF_RenderText_Solid(sdl_handler::get_instance().font,sdl_handler::get_instance().nicknName.c_str(),defaultColor);
        pos[numItems].x = 0;
        pos[numItems].y = 0;
    }

    int show();

    ~gameMenu()
    {
        delete[] labels;
        delete[] menuDisplay;
        delete[] selected;
        delete[] pos;
    }

};

class gameStart
{
protected:
    Board* board;
    list <moves> record;
    bool wasGame;
public:
    gameStart();

    void updateDraw();

    void startWorkGame();

    int endGameWindow(const string endString);

    bool getWasGame()
    {
        return wasGame;
    }

    list <moves> getRecord()
    {
        return record;
    }

    ~gameStart();
};

class Board
{
private:
    figure* **boardArray; // массив доски с фигурами
    char turn; // Контролирует чей ход
    int sizeBoardWidth; // размер доски(x) 55 процентов от экрана
    int sizeBoardHeight;// размер доски(y) 90 процентов от экрана
    const int oneSquareX; // длина 1 клетки
    const int oneSquareY; // высота 1 клетки
    const int firstLineY; // координаы 1-ой линии y
    const int firstLineX; // координаты 1-ой линии x
    SDL_Rect b;
    SDL_Texture* textureBoard;
public:
    //friend class figure;
    Board(const int boardWidth = sdl_handler::get_instance().displayMode.w * 55  / 100, const int boardHeight = sdl_handler::get_instance().displayMode.h * 95 / 100, const string &path = "board.png"):turn(1), sizeBoardWidth(boardWidth),sizeBoardHeight(boardHeight),
        oneSquareX(sizeBoardWidth / 8), oneSquareY(sizeBoardHeight / 8),
        firstLineY((sdl_handler::get_instance().displayMode.h  - sizeBoardHeight) / 2), firstLineX((sdl_handler::get_instance().displayMode.w - sizeBoardWidth) / 2), textureBoard(loadTexture(path, sdl_handler::get_instance().render))
    {
        // Создаем массив 8x8
        boardArray = new figure**[N];
        for(int i = 0; i < N; i++)
        {
            boardArray[i] = new figure*[N];
        }

        b.x = sdl_handler::get_instance().displayMode.w / 2 - sizeBoardWidth / 2;
        b.y = sdl_handler::get_instance().displayMode.h / 2 - sizeBoardHeight / 2;
        b.w = sizeBoardWidth;
        b.h = sizeBoardHeight;
    }

    SDL_Texture* loadTexture(const string &file, SDL_Renderer *ren)
    {
        SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
        if (!texture)
        {
            cout << IMG_GetError();
        }
        return texture;
    }

    char getTurn()
    {
        return turn;
    }

    void setTurn(const char &t)
    {
        turn = t;
    }

    int getSizeBoardWidth()
    {
        return sizeBoardWidth;
    }

    int getSizeBoardHeight()
    {
        return sizeBoardHeight;
    }

    int getOneSquareX()
    {
        return oneSquareX;
    }

    int getOneSquareY()
    {
        return oneSquareY;
    }

    int getFirstLineY()
    {
        return firstLineY;
    }

    int getFirstLineX()
    {
        return firstLineX;
    }

    SDL_Texture* getTextureBoard()
    {
        return textureBoard;
    }

    figure*& getBoardArrayIJ(int i, int j)
    {
        return boardArray[i][j];
    }

    void setBoardArrayIJ(int i, int j, figure* fig = nullptr)
    {
        boardArray[i][j] = fig;
    }

    void drawBoard();

    void resetBoard();

    void initBoard();

    bool conditionCheck(Board*& board, int kingPosX, int kingPosY);

    bool kingCantMove(Board*& board, figure* tempFig, int kingPosX, int kingPosY);

    bool isCheckMate(Board*& board, int kingPosX, int kingPosY, int lastMoveX, int lastMoveY);

    bool isDraw(Board*& board, int kingPosX, int kingPosY);

    figure* pawnTransformation(Board*& board);


    // Деструктор
    ~Board();
};

class figure
{
protected:
    char pieceName;
    char pieceColor; // цвет фигуры
    char advancedTerms; // 1 - Можно взять на проходе, рокировка
    bool itFirstMotion;
    SDL_Rect piece;
    SDL_Texture* textureFigure;
public:
    figure():advancedTerms(0), itFirstMotion(true){};

    void drawFigure()
    {
        SDL_RenderCopy(sdl_handler::get_instance().render, textureFigure, NULL, &piece);
    }

    SDL_Texture* loadTexture(const string &file, SDL_Renderer *ren)
    {
        SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
        if (!texture)
        {
            cout << IMG_GetError();
        }
        return texture;
    }

    char getPieceName()
    {
        return pieceName;
    }

    char getColor()
    {
        return pieceColor;
    }

    void setColor(char const &c)
    {
        pieceColor = c;
    }

    char getAdvancedTerms()
    {
        return advancedTerms;
    }

    void setAdvancdeTerms(const char& aT)
    {
        advancedTerms = aT;
    }

    void move(int x, int y)
    {
        piece.x = x - piece.w / 2;
        piece.y = y - piece.h / 2;
    }

    int getPieceX()
    {
        return piece.x;
    }

    int getPieceY()
    {
        return piece.y;
    }

    int getPieceW()
    {
        return piece.w;
    }

    int getPieceH()
    {
        return piece.h;
    }

    void setPieceX(const int &x)
    {
        piece.x = x;
    }

    void setPieceY(const int &y)
    {
        piece.y = y;
    }

    void setPieceW(const int &value)
    {
        piece.w = value;
    }

    void setPieceH(const int &value)
    {
        piece.h = value;
    }

    virtual string getPath(){
        return nullptr;
    }

    virtual bool canMove(Board*&, int, int, int&, int&,const bool)
    {
        return true;
    }


    bool getItFirstMotion()
    {
        return itFirstMotion;
    }

    void setItFirstMotion(bool firstMove)
    {
        itFirstMotion = firstMove;
    }

    figure* pseudoMove(Board*& board,figure* tempFig, int oldPosX, int oldPosY, int newPosX, int newPosY);

    figure* resetPseudoMove(Board*& board,figure* tempFig, int oldPosX, int oldPosY, int newPosX, int newPosY);

    // Деструктор
    virtual ~figure(){
        SDL_DestroyTexture(textureFigure);
    };

};

class pawn: public figure
{
public:
    pawn(const char color)
    {
        pieceColor = color;
        pieceName = 'p';
        // загружаем текстуру
        textureFigure = loadTexture(getPath().c_str(), sdl_handler::get_instance().render);

    }

    bool canMove(Board*& board, int oldSquareX,int oldSquareY, int &newSquareX, int &newSquareY, const bool figureCheck) override
    {
        if(!figureCheck)
        {
            newSquareX = floor((piece.x - board->getFirstLineX()) / board->getOneSquareX());
            newSquareY = floor((piece.y - board->getFirstLineY()) / board->getOneSquareY());
        }
        // Черные Пешки
        if(!pieceColor)
        {
            if(itFirstMotion)
            {
                if (newSquareX == oldSquareX && newSquareY - oldSquareY == 2)
                {
                    if(!board->getBoardArrayIJ(newSquareY - 1,newSquareX) && !board->getBoardArrayIJ(newSquareY,newSquareX))
                    {
                        advancedTerms = 1;
                        return true;
                    }
                }
            }

            //обычный ход вперед
            if(newSquareX == oldSquareX && newSquareY - oldSquareY == 1)
            {
                if(!board->getBoardArrayIJ(newSquareY,newSquareX))
                {
                    return true;
                }
            }
             // съесть
            if(abs(newSquareX - oldSquareX) == 1 && newSquareY - oldSquareY == 1)
            {
                if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == 1)
                {
                    return true;
                }
                 // взятие на проходе
                if(board->getBoardArrayIJ(newSquareY - 1,newSquareX) && board->getBoardArrayIJ(newSquareY - 1,newSquareX)->getPieceName() == 'p' && board->getBoardArrayIJ(newSquareY - 1,newSquareX)->getAdvancedTerms() == 1 && board->getBoardArrayIJ(newSquareY - 1,newSquareX)->getColor() == 1)
                {
                    board->setBoardArrayIJ(newSquareY - 1, newSquareX, nullptr);
                    return true;
                }
            }
        }
        else // Белые пешки
        {
            if(itFirstMotion)
            {
               if (newSquareX == oldSquareX && oldSquareY - newSquareY == 2)
               {
                   if(!board->getBoardArrayIJ(newSquareY + 1,newSquareX) && !board->getBoardArrayIJ(newSquareY,newSquareX))
                   {
                       advancedTerms = 1;
                       return true;
                   }
               }
            }
            //обычный ход вперед
            if(newSquareX == oldSquareX && oldSquareY - newSquareY == 1)
            {
                if(!board->getBoardArrayIJ(newSquareY,newSquareX))
                {
                    return true;
                }
            }
            // съесть
            if(abs(newSquareX - oldSquareX) == 1 && oldSquareY - newSquareY == 1)
            {
                if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == 0)
                {
                    return true;
                }

                // взятие на проходе
               if(board->getBoardArrayIJ(newSquareY + 1,newSquareX) && board->getBoardArrayIJ(newSquareY + 1,newSquareX)->getPieceName() == 'p' && board->getBoardArrayIJ(newSquareY + 1,newSquareX)->getColor() == 0 && board->getBoardArrayIJ(newSquareY + 1,newSquareX)->getAdvancedTerms() == 1)
               {
                   board->setBoardArrayIJ(newSquareY + 1, newSquareX, nullptr);
                   return true;
               }
            }
        }
        return false;
    }

    string getPath() override
    {
        if (!pieceColor)
        {
            return "pawnBlack.png";
        }
        else
        {
            return "pawnWhite.png";
        }
    }
};

class knight: public figure
{
public:
    knight(const char color)
    {
        pieceColor = color;
        pieceName = 'k';

        textureFigure = loadTexture(getPath().c_str(), sdl_handler::get_instance().render);
    }

    string getPath() override
    {
        if (!pieceColor)
        {
            return "knightBlack.png";
        }
        else
        {
            return "knightWhite.png";
        }
    }

    bool canMove(Board*& board, int oldSquareX,int oldSquareY, int &newSquareX, int &newSquareY, const bool kingCheck) override
    {
        if(!kingCheck)
        {
            newSquareX = floor((piece.x - board->getFirstLineX()) / board->getOneSquareX());
            newSquareY = floor((piece.y - board->getFirstLineY()) / board->getOneSquareY());
        }

        // Верхние и нижние ходы коны
        if(abs(newSquareX - oldSquareX) == 1 && abs(newSquareY - oldSquareY) == 2)
        {
            if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() != this->getColor())
                return true;
            if(!board->getBoardArrayIJ(newSquareY,newSquareX))
                return true;
        }

        // Право и лево
        if(abs(newSquareX - oldSquareX) == 2 && abs(newSquareY - oldSquareY) == 1)
        {
            if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() != this->getColor())
                return true;
            if(!board->getBoardArrayIJ(newSquareY,newSquareX))
                return true;
        }
        return false;
    }
};

class bishop: public figure
{
public:
    bishop(const char color)
    {
        pieceName = 'b';
        pieceColor = color;

        textureFigure = loadTexture(getPath().c_str(), sdl_handler::get_instance().render);
    }


    string getPath() override
    {
        if (!pieceColor)
        {
            return "bishopBlack.png";
        }
        else
        {
            return "bishopWhite.png";
        }
    }

    bool canMove(Board*& board, int oldSquareX,int oldSquareY, int &newSquareX, int &newSquareY, const bool kingCheck) override
    {
        if(!kingCheck)
        {
             newSquareX = floor((piece.x - board->getFirstLineX()) / board->getOneSquareX());
             newSquareY = floor((piece.y - board->getFirstLineY()) / board->getOneSquareY());
        }

        // диагональ
        if(abs(oldSquareX - newSquareX) == abs(oldSquareY - newSquareY))
        {
            for (int i = oldSquareX, j = oldSquareY; i != newSquareX && j != newSquareY; oldSquareX > newSquareX ? i-- : i++, oldSquareY > newSquareY ? j-- : j++)
            {
                if(board->getBoardArrayIJ(j,i) && i != oldSquareX && j != oldSquareY)
                {
                     return false;
                }
            }

            if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == this->getColor())
            {
                return false;
            }

            return true;
        }

        return false;
    }

};

class rook: public figure
{
public:
    rook(const char color)
    {
        pieceName = 'r';
        pieceColor = color;
        textureFigure = loadTexture(getPath().c_str(), sdl_handler::get_instance().render);
    }

    string getPath() override
    {
        if (!pieceColor)
        {
            return "rookBlack.png";
        }
        else
        {
            return "rookWhite.png";
        }
    }

    bool canMove(Board*& board, int oldSquareX,int oldSquareY, int &newSquareX, int &newSquareY, const bool kingCheck) override
    {
        if(!kingCheck)
        {
             newSquareX = floor((piece.x - board->getFirstLineX()) / board->getOneSquareX());
             newSquareY = floor((piece.y - board->getFirstLineY()) / board->getOneSquareY());
        }

        if(newSquareX == oldSquareX)
        {
           for(int i = oldSquareY; i != newSquareY; newSquareY > oldSquareY ? i++ : i--)
           {
                if(board->getBoardArrayIJ(i,newSquareX) && i != oldSquareY)
                {
                     return false;
                }
           }
          if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == this->getColor())
          {
              return false;
          }
          return true;
        }

        // Право и лево
        if(newSquareY == oldSquareY)
        {
           for(int i = oldSquareX; i != newSquareX; newSquareX > oldSquareX ? i++ : i--)
           {
                if(board->getBoardArrayIJ(oldSquareY,i) && i != oldSquareX)
                {
                     return false;
                }
           }
          if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == this->getColor())
          {
              return false;
          }
          return true;
        }

        return false;
    }

};

class queen: public figure
{
public:
    queen(const char color)
    {
        pieceName = 'q';
        pieceColor = color;
        textureFigure = loadTexture(getPath().c_str(), sdl_handler::get_instance().render);
    }
    string getPath() override
    {
        if (!pieceColor)
        {
            return "queenBlack.png";
        }
        else
        {
            return "queenWhite.png";
        }
    }

    bool canMove(Board*& board, int oldSquareX,int oldSquareY, int &newSquareX, int &newSquareY, const bool kingCheck) override
    {
        if(!kingCheck)
        {
            newSquareX = floor((piece.x - board->getFirstLineX()) / board->getOneSquareX());
            newSquareY = floor((piece.y - board->getFirstLineY()) / board->getOneSquareY());
        }

        // Вверх и низ
        if(newSquareX == oldSquareX)
        {
           for(int i = oldSquareY; i != newSquareY; newSquareY > oldSquareY ? i++ : i--)
           {
                if(board->getBoardArrayIJ(i,newSquareX) && i != oldSquareY)
                {
                     return false;
                }
           }
          if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == this->getColor())
          {
              return false;
          }
          return true;
        }

        // Право и лево
        if(newSquareY == oldSquareY)
        {
           for(int i = oldSquareX; i != newSquareX; newSquareX > oldSquareX ? i++ : i--)
           {
                if(board->getBoardArrayIJ(oldSquareY,i) && i != oldSquareX)
                {
                     return false;
                }
           }
          if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == this->getColor())
          {
              return false;
          }
          return true;
        }

        // Диагональ
        if(abs(oldSquareX - newSquareX) == abs(oldSquareY - newSquareY))
        {
            for (int i = oldSquareX, j = oldSquareY; i != newSquareX && j != newSquareY; oldSquareX > newSquareX ? i-- : i++, oldSquareY > newSquareY ? j-- : j++)
            {
                if(board->getBoardArrayIJ(j,i) && i != oldSquareX && j != oldSquareY)
                {
                     return false;
                }
            }

            if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == this->getColor())
            {
                return false;
            }

            return true;
        }

        return false;
    }

};

class king: public figure
{
public: 
    king(const char color){
        pieceName = 'K';
        pieceColor = color;
        textureFigure = loadTexture(getPath().c_str(), sdl_handler::get_instance().render);
    }
    string getPath() override
    {
        if (!pieceColor)
        {
            return "kingBlack.png";
        }
        else
        {
            return "kingWhite.png";
        }
    }

    bool canMove(Board*& board, int oldSquareX,int oldSquareY, int &newSquareX, int &newSquareY, const bool kingCheck) override
    {
        if(!kingCheck)
        {
            newSquareX = floor((piece.x - board->getFirstLineX()) / board->getOneSquareX());
            newSquareY = floor((piece.y - board->getFirstLineY()) / board->getOneSquareY());
        }

       // ход на 1 клетку
       if((abs(oldSquareX - newSquareX) == 1 || oldSquareX == newSquareX) && (abs(oldSquareY - newSquareY) == 1 || oldSquareY == newSquareY))
       {
           if(board->getBoardArrayIJ(newSquareY,newSquareX) && board->getBoardArrayIJ(newSquareY,newSquareX)->getColor() == this->getColor())
           {
               return false;
           }
           return true;
       }


       if(itFirstMotion)
       {
              // короткая рокировка
              if(newSquareY == oldSquareY && newSquareX == oldSquareX + 2)
              {
                  // минимальные условия для рокировки
                  if(board->getBoardArrayIJ(oldSquareY,oldSquareX + 3)->getPieceName() == 'r' && board->getBoardArrayIJ(oldSquareY,oldSquareX + 3)->getItFirstMotion() && !board->getBoardArrayIJ(oldSquareY,oldSquareX + 2) && !board->getBoardArrayIJ(oldSquareY,oldSquareX + 1))
                  {
                      // Проверка на шахи
                      if(board->conditionCheck(board, oldSquareX, oldSquareY) && board->conditionCheck(board, oldSquareX + 1, oldSquareY) && board->conditionCheck(board, oldSquareX + 2, oldSquareY))
                      {
                        board->setBoardArrayIJ(oldSquareY,oldSquareX + 1, board->getBoardArrayIJ(oldSquareY,oldSquareX + 3));
                        board->getBoardArrayIJ(oldSquareY,oldSquareX + 1)->setPieceX(board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(oldSquareY,oldSquareX + 1)->getPieceW() / 2 + (oldSquareX + 1) * board->getOneSquareX());
                        board->getBoardArrayIJ(oldSquareY,oldSquareX + 1)->setPieceY(board->getFirstLineY() + oldSquareY * board->getOneSquareY());
                        board->setBoardArrayIJ(oldSquareY,oldSquareX + 3, nullptr);
                        return true;
                      }
                  }
              }

              // длинная рокировка
              if(newSquareY == oldSquareY && newSquareX == oldSquareX - 2)
              {
                  // минимальные условия для рокировки
                  if(board->getBoardArrayIJ(oldSquareY,oldSquareX - 4)->getPieceName() == 'r' && board->getBoardArrayIJ(oldSquareY,oldSquareX - 4)->getItFirstMotion() && !board->getBoardArrayIJ(oldSquareY,oldSquareX - 1) && !board->getBoardArrayIJ(oldSquareY,oldSquareX - 2) && !board->getBoardArrayIJ(oldSquareY,oldSquareX - 3))
                  {
                      // Проверка на шахи
                      if(board->conditionCheck(board, oldSquareX, oldSquareY) && board->conditionCheck(board, oldSquareX - 1, oldSquareY) && board->conditionCheck(board, oldSquareX - 2, oldSquareY))
                      {
                        board->setBoardArrayIJ(oldSquareY,oldSquareX - 1, board->getBoardArrayIJ(oldSquareY,oldSquareX - 4));
                        board->getBoardArrayIJ(oldSquareY,oldSquareX - 1)->setPieceX(board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(oldSquareY,oldSquareX - 1)->getPieceW() / 2 + (oldSquareX - 1) * board->getOneSquareX());
                        board->getBoardArrayIJ(oldSquareY,oldSquareX - 1)->setPieceY(board->getFirstLineY() + oldSquareY * board->getOneSquareY());
                        board->setBoardArrayIJ(oldSquareY,oldSquareX - 4, nullptr);
                        return true;
                      }
                  }
              }
       }

        return false;
    }

};

class analysisGame
{
private:
    Board* board;
    list <moves> record;
    list <figure*> eatenFigures;
public:
    analysisGame(list <moves> m): record(m)
    {
        board = new Board;
        SDL_SetRenderDrawColor(sdl_handler::get_instance().render, 27, 25, 22, 0);

        board->resetBoard();
        board->initBoard();
    }

    void updateDraw()
    {
        SDL_RenderClear(sdl_handler::get_instance().render);

        board->drawBoard();

        SDL_RenderPresent(sdl_handler::get_instance().render);
    }

    void analysisStart()
    {
        bool isWork = true;
        list<moves>::iterator i = record.begin();
        int positionX, positionY, rookPositionX, rookPositionY; // координаты
        SDL_Event event;
        figure * temp;
        updateDraw();
        while(isWork)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    isWork = false;
                }

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isWork = false;
                }

                if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT) || (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT))
                {
                    if(i != record.end())
                    {
                        if(board->getBoardArrayIJ(i->newY,i->newX))
                        {
                            eatenFigures.push_back(board->getBoardArrayIJ(i->newY,i->newX));
                            temp = board->getBoardArrayIJ(i->newY,i->newX);
                        }
                        // сделать чтобы работала рокировка!!!!
                        board->setBoardArrayIJ(i->newY, i->newX, board->getBoardArrayIJ(i->oldY, i->oldX));
                        board->setBoardArrayIJ(i->oldY,i->oldX, nullptr);
                        positionX = board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(i->newY, i->newX)->getPieceW() / 2 + i->newX * board->getOneSquareX();
                        positionY = board->getFirstLineY() + i->newY * board->getOneSquareY();
                        board->getBoardArrayIJ(i->newY,i->newX)->setPieceX(positionX);
                        board->getBoardArrayIJ(i->newY,i->newX)->setPieceY(positionY);
                        if(i->itWasCastle)
                        {
                            if(i->newX == 6)
                            {
                                board->setBoardArrayIJ(i->newY, i->newX - 1, board->getBoardArrayIJ(i->newY, i->newX + 1));
                                rookPositionX = board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(i->newY, i->newX - 1)->getPieceW() / 2 + (i->newX - 1) * board->getOneSquareX();
                                rookPositionY = board->getFirstLineY() + i->newY * board->getOneSquareY();
                                board->getBoardArrayIJ(i->newY, i->newX - 1)->setPieceX(rookPositionX);
                                board->getBoardArrayIJ(i->newY, i->newX - 1)->setPieceY(rookPositionY);
                                board->setBoardArrayIJ(i->newY, i->newX + 1, nullptr);
                            }
                            else if(i->newX == 2)
                            {
                                board->setBoardArrayIJ(i->newY, i->newX + 1, board->getBoardArrayIJ(i->newY, i->newX - 2));
                                rookPositionX = board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(i->newY, i->newX + 1)->getPieceW() / 2 + (i->newX + 1) * board->getOneSquareX();
                                rookPositionY = board->getFirstLineY() + i->newY * board->getOneSquareY();
                                board->getBoardArrayIJ(i->newY, i->newX + 1)->setPieceX(rookPositionX);
                                board->getBoardArrayIJ(i->newY, i->newX + 1)->setPieceY(rookPositionY);
                                board->setBoardArrayIJ(i->newY, i->newX - 2, nullptr);
                            }
                        }
                        board->setTurn(!board->getTurn());
                        updateDraw();
                        i++;
                    }
                }

                if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT) || (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT))
                {
                    if(i != record.begin())
                    {
                        i--;
                        if(i->itWasCastle)
                        {
                            if(i->newX == 6)
                            {
                                board->setBoardArrayIJ(i->newY, i->newX + 1, board->getBoardArrayIJ(i->newY, i->newX - 1));
                                rookPositionX = board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(i->newY, i->newX + 1)->getPieceW() / 2 + (i->newX + 1) * board->getOneSquareX();
                                rookPositionY = board->getFirstLineY() + i->newY * board->getOneSquareY();
                                board->getBoardArrayIJ(i->newY, i->newX + 1)->setPieceX(rookPositionX);
                                board->getBoardArrayIJ(i->newY, i->newX + 1)->setPieceY(rookPositionY);
                                board->setBoardArrayIJ(i->newY, i->newX - 1, nullptr);
                            }
                            else if(i->newX == 2)
                            {
                                board->setBoardArrayIJ(i->newY, i->newX - 2, board->getBoardArrayIJ(i->newY, i->newX + 1));
                                rookPositionX = board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(i->newY, i->newX - 2)->getPieceW() / 2 + (i->newX - 2) * board->getOneSquareX();
                                rookPositionY = board->getFirstLineY() + i->newY * board->getOneSquareY();
                                board->getBoardArrayIJ(i->newY, i->newX - 2)->setPieceX(rookPositionX);
                                board->getBoardArrayIJ(i->newY, i->newX - 2)->setPieceY(rookPositionY);
                                board->setBoardArrayIJ(i->newY, i->newX + 1, nullptr);
                            }
                        }

                        board->setBoardArrayIJ(i->oldY, i->oldX, board->getBoardArrayIJ(i->newY, i->newX));
                        if(i->figureWasEated)
                        {
                            board->setBoardArrayIJ(i->newY,i->newX, *(--eatenFigures.end()));
                            eatenFigures.pop_back();
                        }
                        else board->setBoardArrayIJ(i->newY,i->newX, nullptr);
                        positionX = board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(i->oldY, i->oldX)->getPieceW() / 2 + i->oldX * board->getOneSquareX();
                        positionY = board->getFirstLineY() + i->oldY * board->getOneSquareY();
                        board->getBoardArrayIJ(i->oldY,i->oldX)->setPieceX(positionX);
                        board->getBoardArrayIJ(i->oldY,i->oldX)->setPieceY(positionY);
                        board->setTurn(!board->getTurn());
                        updateDraw();
                    }
                }

            }
        }
    }

    ~analysisGame()
    {
        for(int i = 0; i < N; i++)
        {
            for(int j = 0; j < N; j++)
            {
                if(board->getBoardArrayIJ(i,j))
                {
                    delete board->getBoardArrayIJ(i,j);
                }
            }
        }
        delete board;
    }
};

int gameMenu::show()
{
    SDL_FillRect(sdl_handler::get_instance().windowSurface,&sdl_handler::get_instance().windowSurface->clip_rect,SDL_MapRGB(sdl_handler::get_instance().windowSurface->format, 27,25,22));

    SDL_Event event;
    while(1)
    {
        SDL_UpdateWindowSurface(sdl_handler::get_instance().window);
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                for(int i = 0; i < numItems; i++)
                {
                    SDL_FreeSurface(menuDisplay[i]);
                }
                return 3;

                case SDL_MOUSEMOTION:
                sdl_handler::get_instance().mouseX = event.motion.x;
                sdl_handler::get_instance().mouseY = event.motion.y;
                for(int i = 0; i < numItems; i++)
                {
                    if(sdl_handler::get_instance().mouseX >= pos[i].x && sdl_handler::get_instance().mouseX <= pos[i].x + pos[i].w && sdl_handler::get_instance().mouseY >= pos[i].y && sdl_handler::get_instance().mouseY <= pos[i].y + pos[i].h)
                    {
                        if(!selected[i])
                        {
                        selected[i] = 1;
                        SDL_FreeSurface(menuDisplay[i]);
                        menuDisplay[i] = TTF_RenderText_Solid(sdl_handler::get_instance().font,labels[i],hoverColor);
                        }
                    }

                    else
                    {
                        if(selected[i])
                        {
                        selected[i] = 0;
                        SDL_FreeSurface(menuDisplay[i]);
                        menuDisplay[i] = TTF_RenderText_Solid(sdl_handler::get_instance().font,labels[i],defaultColor);
                        }
                    }
                }
                break;

                case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    sdl_handler::get_instance().mouseX = event.button.x;
                    sdl_handler::get_instance().mouseY = event.button.y;
                    for(int i = 0; i < numItems; i++)
                    {
                        if(sdl_handler::get_instance().mouseX >= pos[i].x && sdl_handler::get_instance().mouseX <= pos[i].x + pos[i].w && sdl_handler::get_instance().mouseY >= pos[i].y && sdl_handler::get_instance().mouseY <= pos[i].y + pos[i].h)
                        {
                                for(int i = 0; i < numItems; i++)
                                {
                                    SDL_FreeSurface(menuDisplay[i]);
                                }
                                return i;
                        }
                    }
                }
                break;

                case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    for(int i = 0; i < numItems; i++)
                    {
                        SDL_FreeSurface(menuDisplay[i]);
                    }
                    return 3;
                }
            }
        }

        for(int i = 0; i < numItems + 1; i += 1)
        {
            SDL_BlitSurface(menuDisplay[i],NULL,sdl_handler::get_instance().windowSurface,&pos[i]);
        }

    }
}

gameStart::gameStart(): wasGame(false)
{
    board = new Board;
    SDL_SetRenderDrawColor(sdl_handler::get_instance().render, 27, 25, 22, 0);

    board->resetBoard();
    board->initBoard();
}

void gameStart::updateDraw()
{
    SDL_RenderClear(sdl_handler::get_instance().render);

    board->drawBoard();

    SDL_RenderPresent(sdl_handler::get_instance().render);
}

void gameStart::startWorkGame()
{
    bool isWork = true;
    bool isMove = true;
    bool isFigureOn = false;
    bool wasFirstMove = false;
    bool kingCheck = false;
    bool  figureWasEated = false;
    figure* tempFigure;
    moves tmp;


    int oldX, oldY;
    // Новое поле для фигуры
        int newSquareX, newSquareY;
        // Коорды прошлого хода
        int lastMoveSquareX, lastMoveSquareY;

        int newX, newY;

        int mouseSquareX; // На какую клетку по x нажал пользователь
        int mouseSquareY; // На какую клетку по y нажал пользователь
        int flagMouse;

        // координаты для белого и черного короля
        int squareWhiteKingX = 4, squareWhiteKingY = 7;
        int squareBlackKingX = 4, squareBlackKingY = 0;

        SDL_Event event;
        while(isWork)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    isWork = false;
                }

                // в основное меню
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isWork = false;
                }

               if(event.type == SDL_MOUSEBUTTONDOWN)
                {
                  if(event.button.button == SDL_BUTTON_LEFT)
                  {
                      sdl_handler::get_instance().mouseX = event.button.x;
                      sdl_handler::get_instance().mouseY = event.button.y;
                      // Проверяем, чтобы движегие было внутри доски
                      if(sdl_handler::get_instance().mouseX > board->getFirstLineX() && sdl_handler::get_instance().mouseX < board->getFirstLineX() + board->getSizeBoardWidth() && sdl_handler::get_instance().mouseY > board->getFirstLineY() && sdl_handler::get_instance().mouseY < board->getFirstLineY() + board->getSizeBoardHeight())
                      {
                        // считаем на какую клетку было нажатие
                        mouseSquareX = floor((sdl_handler::get_instance().mouseX - board->getFirstLineX()) / board->getOneSquareX());
                        mouseSquareY = floor((sdl_handler::get_instance().mouseY - board->getFirstLineY()) / board->getOneSquareY());
                        flagMouse = 1;
                      }
                  }
                }

                if(event.type == SDL_MOUSEBUTTONUP)
                {
                    if(event.button.button == SDL_BUTTON_LEFT)
                    {
                        if(isFigureOn)
                        {
                            // возвращаем нормальный размер для клетки с фигурой
                            board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->setPieceX(board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getPieceX() + (board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getPieceW() / 2));
                            board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->setPieceY(board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getPieceY() + (board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getPieceH() / 2));
                                // Если можно сделать такой ход
                                if(board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->canMove(board, mouseSquareX, mouseSquareY, newSquareX, newSquareY, kingCheck))
                                {

                                    if (board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getPieceName() == 'K')
                                    {
                                       if(board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getColor())
                                       {
                                          squareWhiteKingX = newSquareX;
                                          squareWhiteKingY = newSquareY;
                                       }
                                       else
                                       {
                                          squareBlackKingX = newSquareX;
                                          squareBlackKingY = newSquareY;
                                       }

                                    }


                                    tmp.figureWasEated = false;
                                    if(board->getBoardArrayIJ(newSquareY,newSquareX))
                                    {
                                         tempFigure = board->getBoardArrayIJ(newSquareY,newSquareX);
                                         figureWasEated = true;
                                         tmp.figureWasEated = true;
                                    }

                                    board->setBoardArrayIJ(newSquareY,newSquareX, board->getBoardArrayIJ(mouseSquareY, mouseSquareX));
                                    board->setBoardArrayIJ(mouseSquareY, mouseSquareX, nullptr);

                                    tmp.itWasCastle = false;
                                    if(board->getBoardArrayIJ(newSquareY,newSquareX)->getPieceName() == 'K')
                                    {
                                        if(abs(newSquareX-mouseSquareX) != 1) tmp.itWasCastle = true;
                                    }


                                   if(board->conditionCheck(board, board->getTurn() ? squareWhiteKingX : squareBlackKingX, board->getTurn() ?  squareWhiteKingY : squareBlackKingY))
                                   {
                                        // Контролирует взятие на проход у пешек
                                        if(wasFirstMove)
                                        {
                                            if(board->getBoardArrayIJ(lastMoveSquareY,lastMoveSquareX) && board->getBoardArrayIJ(lastMoveSquareY,lastMoveSquareX)->getPieceName() == 'p')
                                            {
                                               board->getBoardArrayIJ(lastMoveSquareY,lastMoveSquareX)->setAdvancdeTerms(0);
                                            }
                                        }

                                       // Координаты последнего хода
                                       lastMoveSquareX = newSquareX;
                                       lastMoveSquareY = newSquareY;

                                       newX = board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(newSquareY,newSquareX)->getPieceW() / 2 + newSquareX * board->getOneSquareX();
                                       newY = board->getFirstLineY() + newSquareY * board->getOneSquareY();

                                       if(board->getBoardArrayIJ(newSquareY,newSquareX)->getPieceName() == 'p' && (newSquareY == 7 || newSquareY == 0))
                                       {
                                          delete board->getBoardArrayIJ(newSquareY, newSquareX);
                                          board->setBoardArrayIJ(newSquareY, newSquareX, board->pawnTransformation(board));
                                          board->getBoardArrayIJ(newSquareY, newSquareX)->setPieceW(floor(board->getOneSquareX() * 114 / 100));
                                          board->getBoardArrayIJ(newSquareY, newSquareX)->setPieceH(floor(board->getOneSquareY() * 110 / 100));
                                       }

                                       board->getBoardArrayIJ(newSquareY,newSquareX)->setPieceX(newX);
                                       board->getBoardArrayIJ(newSquareY,newSquareX)->setPieceY(newY);

                                       if(board->getBoardArrayIJ(newSquareY,newSquareX)->getPieceName() == 'K' || 'p' || 'r')
                                       {
                                           if(board->getBoardArrayIJ(newSquareY,newSquareX)->getItFirstMotion())
                                               board->getBoardArrayIJ(newSquareY,newSquareX)->setItFirstMotion(false);
                                       }

                                       wasFirstMove = true;
                                       board->setTurn(!board->getTurn());

                                       // записываем ходы
                                       tmp.oldX = mouseSquareX;
                                       tmp.oldY = mouseSquareY;
                                       tmp.newX = newSquareX;
                                       tmp.newY = newSquareY;
                                       record.push_back(tmp);

                                       // Проверка на ничью
                                       if(board->isDraw(board, board->getTurn() ? squareWhiteKingX : squareBlackKingX, board->getTurn() ?  squareWhiteKingY : squareBlackKingY))
                                       {
                                           updateDraw();
                                           endGameWindow("draw");
                                           return;
                                       }
                                       else
                                       {
                                           // Проверка на мат
                                           if(board->isCheckMate(board, board->getTurn() ? squareWhiteKingX : squareBlackKingX, board->getTurn() ?  squareWhiteKingY : squareBlackKingY, newSquareX, newSquareY))
                                           {
                                               updateDraw();
                                               endGameWindow(board->getTurn() ? "black win" : "white win");
                                               return;
                                           }
                                       }
                                    }
                                    else
                                    {
                                        board->setBoardArrayIJ(mouseSquareY, mouseSquareX,board->getBoardArrayIJ(newSquareY,newSquareX));
                                        if(figureWasEated)  board->setBoardArrayIJ(newSquareY, newSquareX, tempFigure);
                                        else board->setBoardArrayIJ(newSquareY, newSquareX, nullptr);

                                        // возвращаем фигуру на старую позицию + возвращаем коорды короля на старые
                                        board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->setPieceX(oldX);
                                        board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->setPieceY(oldY);
                                        if (board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getPieceName() == 'K')
                                        {
                                           if(board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getColor())
                                           {
                                              squareWhiteKingX = mouseSquareX;
                                              squareWhiteKingY = mouseSquareY;
                                           }
                                           else
                                           {
                                              squareBlackKingX = mouseSquareX;
                                              squareBlackKingY = mouseSquareY;
                                           }

                                        }
                                    }
                                   figureWasEated = false;
                                }
                                else
                                {
                                    board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->setPieceX(oldX);
                                    board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->setPieceY(oldY);
                                }
                                updateDraw();
                                isFigureOn = false;
                        }
                        flagMouse = 0;
                    }
                }

                if(event.type == SDL_MOUSEMOTION && flagMouse == 1)
                {
                    sdl_handler::get_instance().mouseX = event.motion.x;
                    sdl_handler::get_instance().mouseY = event.motion.y;
                    if(board->getBoardArrayIJ(mouseSquareY,mouseSquareX) && board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getColor() == board->getTurn())
                    {
                        oldX = board->getFirstLineX() + board->getOneSquareX() / 2  - board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->getPieceW() / 2 + mouseSquareX * board->getOneSquareX();
                        oldY = board->getFirstLineY() + mouseSquareY * board->getOneSquareY();

                        // Контролируем границы доски
                        if (sdl_handler::get_instance().mouseX < board->getFirstLineX()) sdl_handler::get_instance().mouseX = board->getFirstLineX();
                        if (sdl_handler::get_instance().mouseX > board->getSizeBoardWidth() + board->getFirstLineX()) sdl_handler::get_instance().mouseX = board->getSizeBoardWidth() + board->getFirstLineX();
                        if (sdl_handler::get_instance().mouseY < board->getFirstLineY()) sdl_handler::get_instance().mouseY = board->getFirstLineY();
                        if (sdl_handler::get_instance().mouseY > board->getSizeBoardHeight() + board->getFirstLineY()) sdl_handler::get_instance().mouseY = board->getSizeBoardHeight() + board->getFirstLineY();

                        board->getBoardArrayIJ(mouseSquareY,mouseSquareX)->move(sdl_handler::get_instance().mouseX,sdl_handler::get_instance().mouseY);
                        isMove = true;
                        isFigureOn = true;
                    }
                }

            }

            if(isMove)
            {
                updateDraw();
                isMove = false;
            }
        }
}

int gameStart::endGameWindow(const string endString)
{

   FILE *f;
   if((f = fopen("lastGame", "wb")) != NULL)
   {
       for(list<moves>::iterator i = record.begin(); i != record.end(); i++)
       {
           fwrite(&*i, sizeof(moves), 1, f);
       }
   }
   else cout << "Error" << endl;
   fclose(f);


   wasGame = true;
   SDL_Color colorReset = {255,255,255,0};
   SDL_Color colorMenu = {255,255,255,0};

   int windowW = sdl_handler::get_instance().displayMode.w * 26 / 100; // 26% проц от шир. экрана
   int windowH = sdl_handler::get_instance().displayMode.h * 46 / 100; // 46% проц от выс. экрана
   int windowPosX = sdl_handler::get_instance().displayMode.w / 2 - windowW / 2;
   int windowPosY = sdl_handler::get_instance().displayMode.h / 2 - windowH / 2;

   int messageResultW = windowW / 2;
   int messageResultH = windowH / 5;

   int messageW = windowW * 24 / 100;
   int messageH = windowH * 16 / 100;
   int messagePosXReset = windowPosX + windowW / 2 - messageW / 2 - messageW;
   int messagePosXMenu = windowPosX + windowW / 2 - messageW / 2 + messageW;
   int messagePosY = windowPosY + windowH - messageH * 2;

   SDL_Surface* messageEndGame;
   SDL_Texture* buttonEndGameReset, *buttonEndGameMenu, *buttonEndGameResult;
   // SDL_Texture* buttonEndGame;


   SDL_Rect MessageRectResult = {windowPosX + windowW / 2 - messageResultW / 2, windowPosY + messageResultH, messageResultW, messageResultH};
   SDL_Rect MessageRectReset = {messagePosXReset, messagePosY, messageW, messageH};
   SDL_Rect MessageRectExit = {messagePosXMenu, messagePosY, messageW, messageH};

   messageEndGame = TTF_RenderUTF8_Blended(sdl_handler::get_instance().font, endString.c_str(), {255,255,255,0});
   buttonEndGameResult = SDL_CreateTextureFromSurface(sdl_handler::get_instance().render, messageEndGame);

   messageEndGame = TTF_RenderUTF8_Blended(sdl_handler::get_instance().font, "Reset", colorReset);
   buttonEndGameReset = SDL_CreateTextureFromSurface(sdl_handler::get_instance().render, messageEndGame);

   messageEndGame = TTF_RenderUTF8_Blended(sdl_handler::get_instance().font, "Menu", colorMenu);
   buttonEndGameMenu = SDL_CreateTextureFromSurface(sdl_handler::get_instance().render, messageEndGame);

   SDL_Rect endWindow = {windowPosX, windowPosY, windowW, windowH};

   SDL_SetTextureAlphaMod(board->getTextureBoard(),100);
   updateDraw();

   SDL_SetRenderDrawColor(sdl_handler::get_instance().render, 27,25,22,0);

   SDL_Event event;
   while(1)
   {

       SDL_RenderFillRect(sdl_handler::get_instance().render, &endWindow);

       SDL_RenderCopy(sdl_handler::get_instance().render, buttonEndGameResult, NULL, &MessageRectResult);

       SDL_RenderCopy(sdl_handler::get_instance().render, buttonEndGameReset, NULL, &MessageRectReset);

       SDL_RenderCopy(sdl_handler::get_instance().render, buttonEndGameMenu, NULL, &MessageRectExit);

       SDL_RenderPresent(sdl_handler::get_instance().render);
       while(SDL_PollEvent(&event))
       {
           switch(event.type)
           {
               case SDL_MOUSEMOTION:
               sdl_handler::get_instance().mouseX = event.motion.x;
               sdl_handler::get_instance().mouseY = event.motion.y;
               if(sdl_handler::get_instance().mouseX >= messagePosXReset && sdl_handler::get_instance().mouseX <= messagePosXReset + messageW &&
                  sdl_handler::get_instance().mouseY >= messagePosY && sdl_handler::get_instance().mouseY <= messagePosY + messageH)
               {
                   colorReset = {255,0,0,0};
               }
               else
               {
                   colorReset = {255,255,255,0};
                   if(sdl_handler::get_instance().mouseX >= messagePosXMenu && sdl_handler::get_instance().mouseX <= messagePosXMenu + messageW &&
                      sdl_handler::get_instance().mouseY >= messagePosY && sdl_handler::get_instance().mouseY <= messagePosY + messageH)
                   {
                     colorMenu = {255,0,0,0};
                   }
                   else
                   {
                     colorMenu = {255,255,255,0};
                   }
               }
               break;

               case SDL_MOUSEBUTTONDOWN:
               if(event.button.button == SDL_BUTTON_LEFT)
               {
                    sdl_handler::get_instance().mouseX = event.button.x;
                    sdl_handler::get_instance().mouseY = event.button.y;
                    if(sdl_handler::get_instance().mouseX >= messagePosXReset && sdl_handler::get_instance().mouseX <= messagePosXReset + messageW &&
                       sdl_handler::get_instance().mouseY >= messagePosY && sdl_handler::get_instance().mouseY <= messagePosY + messageH)
                    {
                        SDL_FreeSurface(messageEndGame);
                        SDL_DestroyTexture(buttonEndGameResult);
                        SDL_DestroyTexture(buttonEndGameReset);
                        SDL_DestroyTexture(buttonEndGameMenu);
                        sdl_handler::get_instance().isReset = true;
                        return 1;
                    }
                    else
                    {
                        if(sdl_handler::get_instance().mouseX >= messagePosXMenu && sdl_handler::get_instance().mouseX <= messagePosXMenu + messageW &&
                           sdl_handler::get_instance().mouseY >= messagePosY && sdl_handler::get_instance().mouseY <= messagePosY + messageH)
                        {
                            SDL_FreeSurface(messageEndGame);
                            SDL_DestroyTexture(buttonEndGameResult);
                            SDL_DestroyTexture(buttonEndGameReset);
                            SDL_DestroyTexture(buttonEndGameMenu);
                            return 1;
                        }
                    }
               }
               break;

           }
       }
   }
   return 1;
}

gameStart::~gameStart()
{
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(board->getBoardArrayIJ(i,j))
            {
                delete board->getBoardArrayIJ(i,j);
            }
        }
    }
    delete board;
}

void Board::drawBoard()
{
    SDL_RenderCopy(sdl_handler::get_instance().render, textureBoard, NULL, &b);
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
            if(boardArray[i][j])
            {
                boardArray[i][j]->drawFigure();
            }
    }
}

void Board::initBoard()
{
    // Черные пешки
    for (int i = 0; i < N; i++)
    {
        setBoardArrayIJ(1,i, new pawn(0));
        getBoardArrayIJ(1,i)->setPieceW(floor(oneSquareX * 114 / 100));
        getBoardArrayIJ(1,i)->setPieceH(floor(oneSquareY * 110 / 100));
        getBoardArrayIJ(1,i)->setPieceX(getFirstLineX() + getOneSquareX() / 2  -  getBoardArrayIJ(1,i)->getPieceW() / 2 + i * getOneSquareX());
        getBoardArrayIJ(1,i)->setPieceY(getFirstLineY() + getOneSquareY());
    }

    // enum , массив маску

    // Белые пешки
    for (int i = 0; i < N; i++)
    {
        setBoardArrayIJ(6,i, new pawn(1));
        getBoardArrayIJ(6,i)->setPieceW(floor(oneSquareX * 114 / 100));
        getBoardArrayIJ(6,i)->setPieceH(floor(oneSquareY * 110 / 100));
        getBoardArrayIJ(6,i)->setPieceX(getFirstLineX() + getOneSquareX() / 2  -  getBoardArrayIJ(6,i)->getPieceW() / 2 + i * getOneSquareX());
        getBoardArrayIJ(6,i)->setPieceY(getFirstLineY() + 6 * getOneSquareY());
    }

    setBoardArrayIJ(0,0, new rook(0));
    setBoardArrayIJ(0,1, new knight(0));
    setBoardArrayIJ(0,2, new bishop(0));
    setBoardArrayIJ(0,3, new queen(0));
    setBoardArrayIJ(0,4, new king(0));
    setBoardArrayIJ(0,5, new bishop(0));
    setBoardArrayIJ(0,6, new knight(0));
    setBoardArrayIJ(0,7, new rook(0));

    for(int i = 0; i < N; i++)
    {
        getBoardArrayIJ(0,i)->setPieceW(floor(oneSquareX * 114 / 100));
        getBoardArrayIJ(0,i)->setPieceH(floor(oneSquareY * 98 / 100));
        getBoardArrayIJ(0,i)->setPieceX(getFirstLineX() + getOneSquareX() / 2  - getBoardArrayIJ(0,i)->getPieceW() / 2 + i * getOneSquareX());
        getBoardArrayIJ(0,i)->setPieceY(getFirstLineY());
    }

    setBoardArrayIJ(7,0, new rook(1));
    setBoardArrayIJ(7,1, new knight(1));
    setBoardArrayIJ(7,2, new bishop(1));
    setBoardArrayIJ(7,3, new queen(1));
    setBoardArrayIJ(7,4, new king(1));
    setBoardArrayIJ(7,5, new bishop(1));
    setBoardArrayIJ(7,6, new knight(1));
    setBoardArrayIJ(7,7, new rook(1));


    for(int i = 0; i < N; i++)
    {
        getBoardArrayIJ(7,i)->setPieceW(floor(oneSquareX * 114 / 100));
        getBoardArrayIJ(7,i)->setPieceH(floor(oneSquareY * 98 / 100));
        getBoardArrayIJ(7,i)->setPieceX(getFirstLineX() + getOneSquareX() / 2  - getBoardArrayIJ(7,i)->getPieceW() / 2 + i * getOneSquareX());
        getBoardArrayIJ(7,i)->setPieceY(getFirstLineY() + 7 * getOneSquareY());
    }
}

void Board::resetBoard()
{
    for( int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            setBoardArrayIJ(i,j,nullptr);
        }
    }
}

bool Board::conditionCheck(Board*& board, int kingPosX, int kingPosY)
{
    bool kingCheck = true;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if(board->getBoardArrayIJ(i,j) && board->getBoardArrayIJ(i,j)->getColor() != board->getTurn())
            {
                if(board->getBoardArrayIJ(i,j)->canMove(board,j,i,kingPosX,kingPosY,kingCheck))
                {
                    return false;
                }
            }
        }
    }


    return true;
}

bool Board::kingCantMove(Board*& board, figure* tempFig, int kingPosX, int kingPosY)
{
    // проверяем клетку на выход за границы
    if(kingPosY + 1 < 8 && kingPosX + 1 < 8)
    {
        // если фигура другого цвета или нет фигуры
        if((board->getBoardArrayIJ(kingPosY + 1, kingPosX + 1) && board->getBoardArrayIJ(kingPosY + 1, kingPosX + 1)->getColor() != board->getTurn()) || !board->getBoardArrayIJ(kingPosY + 1, kingPosX + 1))
        {
           tempFig = tempFig->pseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY + 1);
           if(board->conditionCheck(board, kingPosX + 1, kingPosY + 1))
           {
                tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY + 1);
                return false;
           }
           tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY + 1);
        }
    }


    // проверяем клетку на выход за границы
    if(kingPosX + 1 < 8)
    {
        // если фигура другого цвета или нет фигуры
        if((board->getBoardArrayIJ(kingPosY, kingPosX + 1) && board->getBoardArrayIJ(kingPosY, kingPosX + 1)->getColor() != board->getTurn()) || !board->getBoardArrayIJ(kingPosY, kingPosX + 1))
        {
           tempFig = tempFig->pseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY);
           if(board->conditionCheck(board, kingPosX + 1, kingPosY))
           {
                tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY);
                return false;
           }
           tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY);
        }
    }

    // проверяем клетку на выход за границы
    if(kingPosY - 1 > -1 && kingPosX + 1 < 8)
    {
        // если фигура другого цвета или нет фигуры
        if((board->getBoardArrayIJ(kingPosY - 1, kingPosX + 1) && board->getBoardArrayIJ(kingPosY - 1, kingPosX + 1)->getColor() != board->getTurn()) || !board->getBoardArrayIJ(kingPosY - 1, kingPosX + 1))
        {
           tempFig = tempFig->pseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY - 1);
           if(conditionCheck(board, kingPosX + 1, kingPosY - 1))
           {
                tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY - 1);
                return false;
           }
           tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX + 1, kingPosY - 1);
        }
    }

    // проверяем клетку на выход за границы
    if(kingPosY - 1 > -1)
    {
        // если фигура другого цвета или нет фигуры
        if((board->getBoardArrayIJ(kingPosY - 1, kingPosX) && board->getBoardArrayIJ(kingPosY - 1, kingPosX)->getColor() != board->getTurn()) || !board->getBoardArrayIJ(kingPosY - 1, kingPosX))
        {
           tempFig = tempFig->pseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX, kingPosY - 1);
           if(board->conditionCheck(board,  kingPosX, kingPosY - 1))
           {
                tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX, kingPosY - 1);
                return false;
           }
           tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX, kingPosY - 1);
        }
    }


    // проверяем клетку на выход за границы
    if(kingPosY - 1 > -1 && kingPosX - 1 > -1)
    {
        // если фигура другого цвета или нет фигуры
        if((board->getBoardArrayIJ(kingPosY - 1, kingPosX - 1) && board->getBoardArrayIJ(kingPosY - 1, kingPosX - 1)->getColor() != board->getTurn()) || !board->getBoardArrayIJ(kingPosY - 1, kingPosX - 1))
        {
           tempFig = tempFig->pseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY - 1);
           if(board->conditionCheck(board, kingPosX - 1, kingPosY - 1))
           {
                tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY - 1);
                return false;
           }
           tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY - 1);
        }
    }

    // проверяем клетку на выход за границы
    if(kingPosX - 1 > -1)
    {
        // если фигура другого цвета или нет фигуры
        if((board->getBoardArrayIJ(kingPosY, kingPosX - 1) && board->getBoardArrayIJ(kingPosY, kingPosX - 1)->getColor() != board->getTurn()) || !board->getBoardArrayIJ(kingPosY, kingPosX - 1))
        {
           tempFig = tempFig->pseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY);
           if(board->conditionCheck(board, kingPosX - 1, kingPosY))
           {
                tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY);
                return false;
           }
           tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY);
        }
    }

    // проверяем клетку на выход за границы
    if(kingPosY + 1 < 8 && kingPosX - 1 > -1)
    {
        // если фигура другого цвета или нет фигуры
        if((board->getBoardArrayIJ(kingPosY + 1, kingPosX - 1) && board->getBoardArrayIJ(kingPosY + 1, kingPosX - 1)->getColor() != board->getTurn()) || !board->getBoardArrayIJ(kingPosY + 1, kingPosX - 1))
        {
           tempFig = tempFig->pseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY + 1);
           if(board->conditionCheck(board, kingPosX - 1, kingPosY + 1))
           {
                tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY + 1);
                return false;
           }
           tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX - 1, kingPosY + 1);
        }
    }

    // проверяем клетку на выход за границы
    if(kingPosY + 1 < 8)
    {
        // если фигура другого цвета или нет фигуры
        if((board->getBoardArrayIJ(kingPosY + 1, kingPosX) && board->getBoardArrayIJ(kingPosY + 1, kingPosX)->getColor() != board->getTurn()) || !board->getBoardArrayIJ(kingPosY + 1, kingPosX))
        {
           tempFig = tempFig->pseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX, kingPosY + 1);
           if(board->conditionCheck(board, kingPosX, kingPosY + 1))
           {
                tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX, kingPosY + 1);
                return false;
           }
           tempFig = tempFig->resetPseudoMove(board, tempFig, kingPosX, kingPosY, kingPosX, kingPosY + 1);
        }
    }
    return true;
}

bool Board::isCheckMate(Board*& board, int kingPosX, int kingPosY, int lastMoveX, int lastMoveY)
{
    figure* tempFig = nullptr;
    // есть ли шах королю
    if(!board->conditionCheck(board, kingPosX,kingPosY))
    {
        // если король не может двигаться
        if(board->kingCantMove(board,tempFig, kingPosX, kingPosY))
        {
            // можно ли закрыться или cъесть
            for(int i = 0; i < N; i++)
            {
                for(int j = 0; j < N; j++)
                {
                    // фигура другого цвета(не нападающего)
                    if(board->getBoardArrayIJ(i,j) && board->getBoardArrayIJ(i,j)->getColor() == board->getTurn() && board->getBoardArrayIJ(i,j)->getPieceName() != 'K')
                    {
                         int x = lastMoveX, y = lastMoveY;
                         while(x != kingPosX || y != kingPosY)
                         {
                             if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, x, y, true))
                             {
                                 tempFig = tempFig->pseudoMove(board, tempFig, j, i, x, y);
                                 // Проверка на двойной шах
                                 if(board->conditionCheck(board, kingPosX, kingPosY))
                                 {
                                     tempFig = tempFig->resetPseudoMove(board, tempFig, j, i, x, y);
                                     return false;
                                 }
                                 tempFig = tempFig->resetPseudoMove(board, tempFig, j, i, x, y);
                             }

                             if(x != kingPosX) lastMoveX > kingPosX ? x-- : x++;
                             if(y != kingPosY) lastMoveY > kingPosY ? y-- : y++;
                         }
                    }
                }
            }
        }
        else
        {
            return false;
        }
    }
    else // нет шаха королю
    {
        return false;
    }
    return true;
}

bool Board::isDraw(Board*& board, int kingPosX, int kingPosY)
{
    figure* tempFig = nullptr;
    // королю нет шаха
    if(board->conditionCheck(board, kingPosX,kingPosY))
    {
        // если король может ходить
        if(!board->kingCantMove(board,tempFig, kingPosX, kingPosY))
        {
            return false;
        }
        else
        {
            // можно ли сходить фигурой
            int newX, newY;
            for(int i = 0; i < N; i++)
            {
                for(int j = 0; j < N; j++)
                {
                    if(board->getBoardArrayIJ(i,j) && board->getBoardArrayIJ(i,j)->getColor() == board->getTurn() && board->getBoardArrayIJ(i,j)->getPieceName() != 'K')
                    {
                        // проверяем квадрат 3x3 вокруг фигуры + ходы коня и двойной ход пешки
                        if (board->getBoardArrayIJ(i,j)->getPieceName() != 'k')
                        {
                            if(i - 1 > -1 && j - 1 > -1)
                            {
                                newX = j - 1;
                                newY = i - 1;
                                if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                    return false;
                            }

                            if(i - 1 > -1)
                            {
                                newX = j;
                                newY = i - 1;
                                if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                    return false;
                            }

                            if(i - 1 > -1 && j + 1 < 8)
                            {
                                newX = j + 1;
                                newY = i - 1;
                                if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                    return false;
                            }

                            if(j + 1 < 8)
                            {
                                newX = j + 1;
                                newY = i;
                                if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                    return false;
                            }

                            if(i + 1 < 8 && j + 1 < 8)
                            {
                                newX = j + 1;
                                newY = i + 1;
                                if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                    return false;
                            }

                            if(i + 1 < 8)
                            {
                                newX = j;
                                newY = i + 1;
                                if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                    return false;
                            }

                            if(i + 1 < 8 && j - 1 > -1)
                            {
                                newX = j - 1;
                                newY = i + 1;
                                if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                    return false;
                            }

                            if(j - 1 > -1)
                            {
                                newX = j - 1;
                                newY = i;
                                if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                    return false;
                            }

                            // двойной ход пешкой
                            if(board->getBoardArrayIJ(i,j)->getPieceName() == 'p')
                            {
                                if(board->getBoardArrayIJ(i,j)->getItFirstMotion())
                                {
                                    newX = j;
                                    newY = i + 2;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;

                                    newY = i - 2;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }
                            }
                        }
                        else
                        {
                            // верхние ходы коня
                            if(i - 2 > -1)
                            {
                                if(j - 1 > -1)
                                {
                                    newX = j - 1;
                                    newY = i - 2;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }

                                if(j + 1 < 8)
                                {
                                    newX = j + 1;
                                    newY = i - 2;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }

                            }

                            // нижние ходы коня
                            if(i + 2 < 8)
                            {
                                if(j - 1 > -1)
                                {
                                    newX = j - 1;
                                    newY = i + 2;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }

                                if(j + 1 < 8)
                                {
                                    newX = j + 1;
                                    newY = i + 2;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }

                            }

                            // левые ходы коня
                            if(j - 2 > -1)
                            {
                                if(i - 1 > -1)
                                {
                                    newX = j - 2;
                                    newY = i - 1;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }

                                if(i + 1 < 8)
                                {
                                    newX = j - 2;
                                    newY = i + 1;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }

                            }

                            // прав ходы коня
                            if(j + 2 < 8)
                            {
                                if(i - 1 > -1)
                                {
                                    newX = j + 2;
                                    newY = i - 1;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }

                                if(i + 1 < 8)
                                {
                                    newX = j + 2;
                                    newY = i + 1;
                                    if(board->getBoardArrayIJ(i,j)->canMove(board, j, i, newX, newY, true))
                                        return false;
                                }

                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

figure* Board::pawnTransformation(Board*& board)
{
    SDL_Texture* textureKnight, *textureBishop, *textureRook, *textureQueen;
    if(board->getTurn())
    {
        textureKnight = IMG_LoadTexture(sdl_handler::get_instance().render, "knightWhite.png");
        textureBishop = IMG_LoadTexture(sdl_handler::get_instance().render, "bishopWhite.png");
        textureRook = IMG_LoadTexture(sdl_handler::get_instance().render, "rookWhite.png");
        textureQueen = IMG_LoadTexture(sdl_handler::get_instance().render, "queenWhite.png");
    }
    else
    {
        textureKnight = IMG_LoadTexture(sdl_handler::get_instance().render, "knightBlack.png");
        textureBishop = IMG_LoadTexture(sdl_handler::get_instance().render, "bishopBlack.png");
        textureRook = IMG_LoadTexture(sdl_handler::get_instance().render, "rookBlack.png");
        textureQueen = IMG_LoadTexture(sdl_handler::get_instance().render, "queenBlack.png");
    }

    int windowW = sdl_handler::get_instance().displayMode.w * 25 / 100; // 30% проц от шир. экрана
    int windowH = sdl_handler::get_instance().displayMode.h * 10 / 100; // 15% проц от выс. экрана
    int windowPosX = sdl_handler::get_instance().displayMode.w / 2 - windowW / 2;
    int windowPosY = sdl_handler::get_instance().displayMode.h / 2 - windowH / 2;

    int pieceW = windowW * 25 / 100; // 25 процента от windowh
    int pieceH = windowH * 90 / 100;
    int knightPosX = sdl_handler::get_instance().displayMode.w / 2 - windowW / 2;
    int bishopPosX = sdl_handler::get_instance().displayMode.w / 2 - windowW / 2 + pieceW;
    int rookPosX = sdl_handler::get_instance().displayMode.w / 2 - windowW / 2 + pieceW * 2;
    int queenPosX = sdl_handler::get_instance().displayMode.w / 2 - windowW / 2 + pieceW * 3;
    int piecePosY = windowPosY + windowH / 2 - pieceH / 2;

    SDL_Rect pieceKnight = {knightPosX, piecePosY, pieceW, pieceH};
    SDL_Rect pieceBishop = {bishopPosX, piecePosY, pieceW, pieceH};
    SDL_Rect pieceRook = {rookPosX, piecePosY, pieceW, pieceH};
    SDL_Rect pieceQueen = {queenPosX, piecePosY, pieceW, pieceH};

    SDL_Rect endWindow = {windowPosX, windowPosY, windowW, windowH};

    SDL_SetRenderDrawColor(sdl_handler::get_instance().render, 27,25,22,0);

    SDL_Event event;
    while(1)
    {

        SDL_RenderFillRect(sdl_handler::get_instance().render, &endWindow);

        SDL_RenderCopy(sdl_handler::get_instance().render, textureKnight, NULL, &pieceKnight);
        SDL_RenderCopy(sdl_handler::get_instance().render, textureBishop, NULL, &pieceBishop);
        SDL_RenderCopy(sdl_handler::get_instance().render, textureRook, NULL, &pieceRook);
        SDL_RenderCopy(sdl_handler::get_instance().render, textureQueen, NULL, &pieceQueen);

        SDL_RenderPresent(sdl_handler::get_instance().render);
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {

                case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                     sdl_handler::get_instance().mouseX = event.button.x;
                     sdl_handler::get_instance().mouseY = event.button.y;
                     if(sdl_handler::get_instance().mouseX >= knightPosX && sdl_handler::get_instance().mouseX <= knightPosX + pieceW &&
                        sdl_handler::get_instance().mouseY >= piecePosY && sdl_handler::get_instance().mouseY <= piecePosY + pieceH)
                     {
                         SDL_DestroyTexture(textureKnight);
                         SDL_DestroyTexture(textureBishop);
                         SDL_DestroyTexture(textureRook);
                         SDL_DestroyTexture(textureQueen);
                         return new knight(board->getTurn());
                     }
                     else
                     {
                         if(sdl_handler::get_instance().mouseX >= bishopPosX && sdl_handler::get_instance().mouseX <= bishopPosX + pieceW &&
                            sdl_handler::get_instance().mouseY >= piecePosY && sdl_handler::get_instance().mouseY <= piecePosY + pieceH)
                         {
                             SDL_DestroyTexture(textureKnight);
                             SDL_DestroyTexture(textureBishop);
                             SDL_DestroyTexture(textureRook);
                             SDL_DestroyTexture(textureQueen);
                             return new bishop(board->getTurn());
                         }
                         else
                         {
                             if(sdl_handler::get_instance().mouseX >= rookPosX && sdl_handler::get_instance().mouseX <= rookPosX + pieceW &&
                                sdl_handler::get_instance().mouseY >= piecePosY && sdl_handler::get_instance().mouseY <= piecePosY + pieceH)
                             {
                                 SDL_DestroyTexture(textureKnight);
                                 SDL_DestroyTexture(textureBishop);
                                 SDL_DestroyTexture(textureRook);
                                 SDL_DestroyTexture(textureQueen);
                                 return new rook(board->getTurn());
                             }
                             else
                             {
                                 if(sdl_handler::get_instance().mouseX >= queenPosX && sdl_handler::get_instance().mouseX <= queenPosX + pieceW &&
                                    sdl_handler::get_instance().mouseY >= piecePosY && sdl_handler::get_instance().mouseY <= piecePosY + pieceH)
                                 {
                                     SDL_DestroyTexture(textureKnight);
                                     SDL_DestroyTexture(textureBishop);
                                     SDL_DestroyTexture(textureRook);
                                     SDL_DestroyTexture(textureQueen);
                                     return new queen(board->getTurn());
                                 }
                             }
                         }
                     }
                }
                break;

            }
        }
    }
}

Board::~Board()
{
    SDL_DestroyTexture(textureBoard);
    delete[] boardArray;
}

figure* figure::pseudoMove(Board*& board,figure* tempFig, int oldPosX, int oldPosY, int newPosX, int newPosY)
{
    if(board->getBoardArrayIJ(newPosY, newPosX)) tempFig = board->getBoardArrayIJ(newPosY, newPosX);
    board->setBoardArrayIJ(newPosY, newPosX, board->getBoardArrayIJ(oldPosY, oldPosX));
    board->setBoardArrayIJ(oldPosY, oldPosX, nullptr);
    return tempFig;
}

figure* figure::resetPseudoMove(Board*& board, figure* tempFig, int oldPosX, int oldPosY, int newPosX, int newPosY)
{
    board->setBoardArrayIJ(oldPosY, oldPosX, board->getBoardArrayIJ(newPosY, newPosX));
    if(tempFig) board->setBoardArrayIJ(newPosY, newPosX, tempFig);
    else board->setBoardArrayIJ(newPosY, newPosX, nullptr);
    tempFig = nullptr;
    return tempFig;
}

int main(int argc, char* args[])
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    bool aplicationWork = true;
    bool wasGame = false;
    list <moves> tmp;
    char* blank[1] = {"Empty"};
    char* menuLabels[4] = {"Play", "Game Review", "How To Play", "Exit"};
    char* previewLabels[3] = {"Alexandr Malyshev I504B", "BGTU VOEMMEH", "Game: Chess"};
    char* about[31] = {"Gameplay: white moves first, then players alternate moves. Making a move is required; it is not legal to skip a move, even when having",
                         "to move is detrimental. Play continues until a king is checkmated, a player resigns, or a draw is declared, as explained below. In addition, if",
                         "the game is being played under a time control a player who exceeds the time limit loses the game. The official chess rules do not include",
                         "a procedure for determining who plays White. Instead, this decision is left open to tournament-specific rules (e.g. a Swiss system",
                         "tournament or round-robin tournament) or, in the case of non-competitive play, mutual agreement, in which case some",
                         "kind of random choice is often employed.",
                         " ",
                         "Movement: - The king moves exactly one square horizontally, vertically, or diagonally. A special move with the king known as castling",
                         "is allowed only once per player, per game.",
                         "- A rook moves any number of vacant squares horizontally or vertically. It also is moved when castling.",
                         "- A bishop moves any number of vacant squares diagonally.",
                         "- The queen moves any number of vacant squares horizontally, vertically, or diagonally.",
                         "- A knight moves to the nearest square not on the same rank, file, or diagonal.",
                         "- A pawn moves straight forward one square, if that square is vacant. If it has not yet moved, a pawn also has the option of moving",
                         "two squares straight forward, provided both squares are vacant. Pawns cannot move backwards. A pawn, unlike other pieces, captures",
                         " differently from how it moves. A pawn can capture an enemy piece on either of the two squares diagonally in front of the pawn (but cannot",
                         " move to those squares if they are vacant). The pawn is also involved in the two special moves en passant and promotion.",
                         " ",
                         "Castling consists of moving the king two squares towards a rook, then placing the rook on the other side of the king, adjacent to it. Castling is only permissible if all of",
                         "the following conditions hold:  The king and rook involved in castling must not have previously moved, there must be no pieces between the king and the rook, the king",
                         "may not currently be in check, nor may the king pass through or end up in a square that is under attack by an enemy piece, he castling must be kingside or queenside as",
                         "shown in the diagram. En passant: When a pawn advances two squares from its original square and ends the turn adjacent to a pawn of the opponent's on the same",
                         "rank, it may be captured by that pawn of the opponent's, as if it had moved only one square forward. This capture is only legal on the opponent's next move immediately",
                         "following the first pawn's advance. The diagrams on the right demonstrate an instance of this: if the white pawn moves from a2 to a4, the black pawn on b4 can capture",
                         "it en passant, moving from b4 to a3 while the white pawn on a4 is removed from the board. Pawn promotion: if a player advances a pawn to its eighth rank, the pawn is",
                         "then promoted (converted) to a queen, rook, bishop, or knight of the same color at the choice of the player (a queen is usually chosen). The choice is not limited",
                         "to previously captured pieces. Hence it is theoretically possible for a player to have up to nine queens or up to ten rooks, bishops, or knights if all of their pawns",
                         "are promoted. If the desired piece is not available, the player must call the arbiter to provide the piece. Check and checkmate: A king is in check when it is under attack by ",
                         "at least one enemy piece. A piece unable to move because it would place its own king in check (it is pinned against its own king) may still deliver",
                         "check to the opposing player. It is illegal to make a move that places or leaves one's king in check. If it is not possible to get out of check,",
                         "the king is checkmated and the game is over."};

    SDL_Color defColor = {255,255,255,0};
    SDL_Color hover = {255,0,0,0};

    // инит sdl
    if (!sdl_handler::get_instance().init()) {
        sdl_handler::get_instance().quit();
        system("pause");
        return 1;
    }

    TTF_Font* aboutFont = TTF_OpenFont("DroidSans.ttf", 25);

    FILE *f;
    if((f = fopen("lastGame", "rb")) != NULL)
    {
        wasGame = true;
        moves tempMoves;
        while(fread(&tempMoves, sizeof(moves), 1, f))
        {
           tmp.push_back(tempMoves);
        }
    }
    fclose(f);

    secondaryMenu* previewMenu = new secondaryMenu(3, previewLabels, defColor,sdl_handler::get_instance().font,1,1);
    previewMenu->show();
    delete previewMenu;

    enterNickName* nick = new enterNickName(defColor);
    nick->enter();
    delete nick;

    while(aplicationWork)
    {
        if(!sdl_handler::get_instance().isReset)
        {
            gameMenu* menu = new gameMenu(4, menuLabels, defColor, hover);
            int chosenItem = menu->show();
            if(chosenItem == 0)
            {
                delete menu;
                gameStart* game = new gameStart;
                game->startWorkGame();
                tmp = game->getRecord();
                if(!wasGame) wasGame = game->getWasGame();
                delete game;
            }
            else if (chosenItem == 1 && wasGame)
            {
                delete menu;
                analysisGame* analysis = new analysisGame(tmp);
                analysis->analysisStart();
                delete analysis;
            }
            else if (chosenItem == 1 && !wasGame)
            {
                delete menu;
                secondaryMenu* empty = new secondaryMenu(1, blank, defColor, sdl_handler::get_instance().font,1,1);
                empty->show();
                delete empty;
            }
            else if (chosenItem == 2)
            {
                delete menu;
                secondaryMenu* howToPlay = new secondaryMenu(31, about, defColor, aboutFont,0,0);
                howToPlay->show();
                delete howToPlay;
            }
            else if (chosenItem == 3)
            {
                delete menu;
                aplicationWork = false;
            }
        }
        else
        {
            sdl_handler::get_instance().isReset = false;
            gameStart* game = new gameStart;
            game->startWorkGame();
            delete game;
        }
    }

    sdl_handler::get_instance().quit();
    return 0;
}
