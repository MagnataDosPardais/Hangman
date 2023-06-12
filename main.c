#include "raylib.h"
#include "stdio.h"
#include "math.h"

#define SCR_W  800
#define SCR_H  600


typedef struct {  //Style
    const char* name;
    Color bg;
    Color commonText;
    Color activeText;
    Color unactiveText;
    Color disableText;
    Color gallow;
    Color hangman;
} Style;
void Style_init(Style* s, const char* n, Color b, Color c, Color a, Color u, Color d, Color g, Color h) {
    s->name = n;
    s->bg = b;
    s->commonText = c;
    s->activeText = a;
    s->unactiveText = u;
    s->disableText = d;
    s->gallow = g;
    s->hangman = h;
}

typedef struct {  //Text
    int id;
    const char* string;
    Font font;
    float fontSize;
    Vector2 textSize;    
    Vector2 pos;
    int state;
    Color color;
} Text;
void Text_init(Text* t, int i, const char* s, Font f, float fs, Vector2 xy, Vector2 ts, int st) { //MeasureText -> 0 anyways
    t->string = s;
    t->id = i;
    t->font = f;
    t->fontSize = fs;
    t->textSize = ts;
    t->pos = xy;
    t->state = st; //0=BG, 1=CT, 2=AT, 3=UT, 4=DT, 5=GL, 6=HM;
}
void Text_centerX(Text* t, float sw, Vector2 rf) { t->pos.x = (int)((sw * rf.x / rf.y) - (t->textSize.x / 2)); }
void Text_centerY(Text* t, float sw, Vector2 rf) { t->pos.x = (int)((sw * rf.x / rf.y) - (t->textSize.y / 2)); }
void Text_show(Text* t) { DrawTextEx(t->font,t->string,t->pos,t->fontSize,t->fontSize/10,t->color); }
void Text_activate(Text* t, int v) { if(v == t->id) t->state = 1; else t->state = 2; }
void Text_updateColor(Text* t[50], Style st, int len) {
    for(int i = 0; i < len; i++) {
        switch(t[i]->state) {
            case(0):
                t[i]->color = st.bg;
                break;
            case(1):
                t[i]->color = st.commonText;
                break;
            case(2):
                t[i]->color = st.activeText;
                break;
            case(3):
                t[i]->color = st.unactiveText;
                break;
            case(4):
                t[i]->color = st.disableText;
                break;
            case(5):
                t[i]->color = st.gallow;
                break;
            case(6):
                t[i]->color = st.hangman;
                break;
            default:
                t[i]->color = BLACK;
        }
    }
}

typedef struct {  //Word
    const char* string;
    const char* tip;
    int helps;
    int lenght;
    int faults;
} Word;


int main() {
    int gameState = 1;
    int selectMain = 1; //1-3
    int selectOptn = 4; //4-?
    int selectStyle = 0;
    Font regFont = GetFontDefault();
    
    //int key = GetCharPressed();
    //Rectangle textBox = { screenWidth/2.0f - 100, 180, 225, 50 };
    Style styles[10];
    Style_init( //Shoyo
        &styles[0],
        "SHOYO",
        (Color){32,28,55,255},
        (Color){248,248,242,255},
        (Color){73,255,81,255},
        (Color){171,119,255,255},
        (Color){54,48,86,255},
        (Color){189,112,39,255},
        (Color){97,234,217,255}
    );
    Style_init( //Obsidian
        &styles[1],
        "OBSIDIAN",
        (Color){23,7,14,255},
        (Color){248,248,242,255},
        (Color){47,170,246,255},
        (Color){30,205,134,255},
        (Color){79,91,97,255},
        (Color){55,216,157,255},
        (Color){250,208,71,255}
    );
    Style_init( //'84
        &styles[2],
        "SYNTHWAVE'84",
        (Color){36,27,47,255},
        (Color){254,222,93,255},
        (Color){54,249,246,255},
        (Color){26,134,214,255},
        (Color){94,92,105,255},
        (Color){243,133,57,255},
        (Color){114,241,184,255}
    );
    Style_init( //Kirino
        &styles[3],
        "KIRINO",
        (Color){18,30,28,255},
        (Color){248,248,242,255},
        (Color){238,196,94,255},
        (Color){73,231,208,255},
        (Color){30,50,47,255},
        (Color){138,217,255,255},
        (Color){88,151,89,255}
    );
    Style_init( //Umi
        &styles[4],
        "UMI",
        (Color){21,23,60,255},
        (Color){88,183,255,255},
        (Color){225,49,116,255},
        (Color){255,246,179,255},
        (Color){48,51,100,255},
        (Color){134,219,253,255},
        (Color){189,132,221,255}
    );
    Style_init( //Rias
        &styles[5],
        "RIAS",
        (Color){16,0,0,255},
        (Color){238,237,232,255},
        (Color){124,23,31,255},
        (Color){192,71,80,255},
        (Color){147,108,107,255},
        (Color){92,140,151,255},
        (Color){39,192,180,255}
    );
    Style_init( //Satsuki
        &styles[6],
        "SATSUKI",
        (Color){255,255,255,255},
        (Color){77,77,74,255},
        (Color){17,54,185,255},
        (Color){43,159,230,255},
        (Color){229,229,238,255},
        (Color){25,169,143,255},
        (Color){121,7,76,255}
    ); 
    Style_init( //Mai
        &styles[7],
        "MAI",
        (Color){0,0,0,255},
        (Color){241,236,242,255},
        (Color){79,82,135,255},
        (Color){123,124,163,255},
        (Color){144,164,184,255},
        (Color){25,118,239,255},
        (Color){76,148,214,255}
    );
    Style_init( //HCB
        &styles[8],
        "BLACK HIGH CONTRAST",
        BLACK,
        RAYWHITE,
        ORANGE,
        LIGHTGRAY,
        DARKGRAY,
        LIGHTGRAY,
        RAYWHITE
    );
    Style_init( //HCW
        &styles[9],
        "WHITE HIGH CONTRAST",
        RAYWHITE,
        BLACK,
        ORANGE,
        DARKGRAY,
        LIGHTGRAY,
        DARKGRAY,
        BLACK
    );
    
    
    Text titleText;
    Text_init(&titleText,0,"Jogo da Forca",regFont,52,(Vector2){0,10},(Vector2){385,47},1);
    Text_centerX(&titleText,SCR_W,(Vector2){1,2});
    
        Text playText;
        Text_init(&playText,1,"Jogar",regFont,40,(Vector2){20,500},(Vector2){116,36},3);
        Text_centerX(&playText,SCR_W,(Vector2){1,6});
    
        Text optionText;
        Text_init(&optionText,2,"Opçoes",regFont,40,(Vector2){220,500},(Vector2){144,36},3);
        Text_centerX(&optionText,SCR_W,(Vector2){1,2});
        
        Text exitText;
        Text_init(&exitText,3,"Sair",regFont,40,(Vector2){420,500},(Vector2){80,28},3);
        Text_centerX(&exitText,SCR_W,(Vector2){5,6});
        
        Text creditText;
        Text_init(&creditText,-1,"by: Leonardo Kenji Ueze & Marco Antonio Zerbielli Bee",regFont,14,(Vector2){7,580},(Vector2){391,13},4);
    
    Text optionTitleText;
    Text_init(&optionTitleText,0,"Opçoes",regFont,40,(Vector2){0,10},(Vector2){144,36},1);
    Text_centerX(&optionTitleText,SCR_W,(Vector2){1,2});
    
        Text themeText;
        Text_init(&themeText,4,"Tema: ",regFont,28,(Vector2){10,80},(Vector2){92,19},3);
    
    Text texts[50] = {titleText,playText,optionText,exitText,creditText,optionTitleText,themeText};
    Text_updateColor(&texts,styles[selectStyle],7);

    
    InitWindow(SCR_W, SCR_H, "Jogo da forca");
    SetTargetFPS(60);
    while(!WindowShouldClose() && gameState != 0) {
        BeginDrawing();
            if(IsKeyPressed(KEY_BACKSPACE)) gameState = 1;
            switch(gameState) {
                case(1):
                    ClearBackground(styles[selectStyle].bg);
                    Text_show(&titleText);
                    Text_show(&playText);
                    Text_show(&optionText);
                    Text_show(&exitText);
                    Text_show(&creditText);
                    
                    if(IsKeyPressed(KEY_RIGHT)) selectMain++;
                    if(IsKeyPressed(KEY_LEFT)) selectMain--;
                    if(IsKeyPressed(KEY_ENTER)) {
                        switch(selectMain) {
                            case(1):
                                gameState = 2;
                                break;
                            case(2):
                                gameState = 3;
                                break;
                            case(3):
                                gameState = 0;
                                break;
                            default:
                                break;
                        }
                    }
                    selectMain < 1
                      ? selectMain = 3
                      : selectMain;
                    selectMain > 3
                      ? selectMain = 1
                      : selectMain;
                    
                    Text_activate(&playText, selectMain);
                    Text_activate(&optionText, selectMain);
                    Text_activate(&exitText, selectMain);
                    break;
                case(2):
                    ClearBackground(styles[selectStyle].bg);
                    DrawText("Tela de jogo",10,10,32,styles[selectStyle].activeText);
                    break;
                case(3):
                    ClearBackground(styles[selectStyle].bg);
                    Text_show(&optionTitleText);
                    Text_show(&themeText);
                    DrawTextEx(
                        regFont,
                        styles[selectStyle].name,
                        (Vector2){themeText.pos.x+themeText.textSize.x,themeText.pos.y},
                        themeText.fontSize,
                        themeText.fontSize/10,
                        themeText.color
                    );
                    
                    if(IsKeyPressed(KEY_UP))   selectOptn++;
                    if(IsKeyPressed(KEY_DOWN)) selectOptn--;
                    if(IsKeyPressed(KEY_RIGHT) && selectOptn == 4) {
                        selectStyle++;
                    }
                    if(IsKeyPressed(KEY_LEFT)  && selectOptn == 4) {
                        selectStyle--;
                    }

                    selectOptn < 4
                      ? selectOptn = 4
                      : selectOptn;
                    selectOptn > 4
                      ? selectOptn = 4
                      : selectOptn;
                    selectStyle < 0
                      ? selectStyle = 9
                      : selectStyle;
                    selectStyle > 9
                      ? selectStyle = 0
                      : selectStyle;
                    
                    Text_activate(&themeText, selectOptn);
                    break;
                default:
                    gameState = 1;
            }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
