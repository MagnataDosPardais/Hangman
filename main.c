/*
    *Jogo da Forca em C com Raylib
    *Desenvolvido por: Leonardo Kenji Ueze && Marco Antônio Zeribiell Bee
    *Orientador: Profº. Ph.D. Thiago Henrique Silva
    *Instituição: UTFPR_Curitiba - Computação 1
    *Data: 26/06/2023
*/

#include "raylib.h"     //Biblioteca da GUI 
#include "stdlib.h"     //Biblioteca para a função de aleatoridade
#include "stdbool.h"    //Biblioteca para a introdução do tipo booleano
#include "string.h"     //Biblioteca para a introdução do tipo string


#define SCR_W     800   //Largura da tela (x)
#define SCR_H     600   //Altura da tela (y)
#define LEN_TEXTS 18    //Quantidade de "textos"
#define LEN_STYLE 10    //Quantidade de "temas"
#define LEN_WORDS 15    //Quantidade de "palavras"


typedef struct {        //Define o tipo Style (Tema de cores)
    const char* name;       //Nome do estilo
    Color bg;               //Cor de fundo
    Color commonText;       //Cor do  texto comum
    Color activeText;       //Cor do texto ativo
    Color unactiveText;     //Cor do texto inativo
    Color disableText;      //Cor do texto desativado
    Color gallow;           //Cor da forca
    Color hangman;          //Cor do  enforcado
} Style;
void Style_init(Style* s, const char* n, Color b, Color c, Color a, Color u, Color d, Color g, Color h) {   //Função de iniciação do estilo
    s->name = n;
    s->bg = b;
    s->commonText = c;
    s->activeText = a;
    s->unactiveText = u;
    s->disableText = d;
    s->gallow = g;
    s->hangman = h;
}

typedef struct {  //Define o tipo Text (Textos que serão mestrados na GUI)
    int id;             //Identificador do texto
    const char* string; //Caracteres do  texto
    Font font;          //Fonte do texto
    float fontSize;     //Tamanho da fonte
    Vector2 textSize;   //Tamanho do texto (em px)
    Vector2 pos;        //Posição do texto
    int state;          //Condição do texto para a cor: 0 == bg; 1 == commonnText; 2 == activeText; 3 == unactiveText; 4 == disableText; 5 == gallow; 6 == hangman;
    Color color;        //Cor do texto
} Text;
void Text_init(Text* t, int i, const char* s, Font f, float fs, Vector2 xy, Vector2 ts, int st) {   //Função de iniciação do texto
    t->string = s;
    t->id = i;
    t->font = f;
    t->fontSize = fs;
    t->textSize = ts;   /*Issue: MeasureTextEx <- 0*/
    t->pos = xy;
    t->state = st;
}
void Text_centerX(Text* t, int sw, double rf) { t->pos.x = (int)sw * rf - t->textSize.x / 2; }      //Centralizar texto horizontalmente nas repartições da tela
void Text_centerY(Text* t, int sh, double rf) { t->pos.y = (int)sh * rf - t->textSize.y / 2; }      //Centralizar texto verticalmente nas repartições da tela
void Text_show(Text t) { DrawTextEx(t.font,t.string,t.pos,t.fontSize,t.fontSize/10,t.color); }      //Mostrar texto na tela
void Text_cpFormatAndShow(Text t, const char* s) {                                                  //Copia a formatação de um texto, aplica em outra string e mostra
    DrawTextEx(
        t.font,
        s,
        (Vector2){t.pos.x+t.textSize.x,t.pos.y},
        t.fontSize,
        t.fontSize/10,
        t.color
    );
}
void Text_activate(Text* t, int v) { if(v == t->id) { t->state = 2; } else { t->state = 3; } }      //Ativa / desativa os textos
void Text_disable(Text* t, int v) { if(v == t->id) { t->state = 4; } else { t->state = 3; } }       //Desativa / desabilita os textos
void Text_hide(Text* t, bool h) { if(h) { t->state = -1; } else { t->state = 1; } }                 //Esconte / revela os textos comuns
void Text_updateColor(Text* t[], Style st, int len) {                                               //Atualiza as cores de todos os textos
    for(int i = 0; i < len; i++) {
        switch(t[i]->state) {
            case(0): t[i]->color = st.bg; break;
            case(1): t[i]->color = st.commonText; break;
            case(2): t[i]->color = st.activeText; break;
            case(3): t[i]->color = st.unactiveText; break;
            case(4): t[i]->color = st.disableText; break;
            case(5): t[i]->color = st.gallow; break;
            case(6): t[i]->color = st.hangman; break;
            default: t[i]->color = (Color){0,0,0,0};        //Condição de ocultamento do texto
        }
    }
}

typedef struct {  //Define o tipo Word (Palavras do jogo)
    const char* string; //String da palavra
    const char* tip;    //Dica da palavra
    int tipSize;        //Tamanho da dica (em px)
    bool reveal[22];    //Lista de revelação de letras
    int helps[2];       //Ajudas [valor original, restantes]
    int lenght;         //Quantidade de caracteres da palavra
} Word;
void Word_init(Word* w, const char* s, const char* t, int ts, int h) {  //Função de iniciação da pavlavra
    w->string = s;
    w->tip = t;
    w->tipSize = ts;
    for(int n = 0; n < 22; n++) { w->reveal[n] = false; }
    for(int n = 0; n < strlen(w->string); n++) { if(w->string[n] == 32) { w->reveal[n] = true; } } //32 == ' '
    w->helps[0] = h;
    w->helps[1] = h;
    w->lenght = strlen(s);
}
void Word_showUnderline(Word w, Vector2 p, Color c) {                   //Mostrar a linha sob a palavra
    const int size     = w.lenght > 14 ? 30 : 40;               //Tamanho do traço varia com o tamanho da palavra
    const int gap      = w.lenght > 14 ? 6 : 10;                //Espaçamento
    const int fullSize = (size*w.lenght) + (gap*(w.lenght-1));  //Tramanho total do desenho
    const int rad      = w.lenght > 14 ? 3 : 4;                 //Raio do círculo
    for(int l = 0; l < w.lenght; l++) {
        w.string[l] == 32
          ? DrawCircle((p.x+(size+gap)*l)-(fullSize/2)+(size+rad)/2, p.y-size/2, rad, c)    //Quando o caractere for um espaço => ponto
          : DrawRectangle((p.x+(size+gap)*l)-(fullSize/2), p.y, size, 3, c);                //Quando o caractere for uma letra => traço
    }
}  
void Word_showWord(Word w, Vector2 p, Font f, Style s, bool ph) {       //Mostrar a palavra
    //ph == Letras fantasmas, ou seja, no fim de jogo.
    const int size     = w.lenght > 14 ? 30 : 40;               //Tamanho da fonte
    const int gap      = w.lenght > 14 ? 6 : 10;                //Espaçameno
    const int fullSize = (size*w.lenght) + (gap*(w.lenght-1));  //Tamanho da palavra
    for(int l = 0; l < w.lenght; l++) {
        if((w.string[l] != 32 && w.reveal[l]) ||  ph) {
            DrawTextEx(
                f,
                TextFormat("%c",w.string[l]),
                (Vector2){
                    (p.x+(size+gap)*l)-(fullSize/2)+(size/5), 
                    p.y-(size/1.5)
                },
                size,
                3,
                (ph && !w.reveal[l]) ? s.disableText : s.commonText //Em caso de derrota, mostrar as letras restantes em cor disableText.
            );  
        }
        else { DrawTextEx(f," ",(Vector2){0,0},0,0,(Color){0,0,0,0}); }
    }
}
bool Word_haveHelps(Word w) { return w.helps[1] > 0; }                  //Verifica se há ajudas disponíveis
bool Word_isComplete(Word w) {                                          //Verifica se a palavra foi totalmente revelada
    int cnt = 0;
    for(int i = 0; i < w.lenght; i++) { (w.reveal[i]) ? cnt++ : cnt; }
    return cnt == w.lenght;
}
void Word_restoreOne(Word* w) {                                         //Oculta os caracteres de uma palavra e retaura as ajudas
    for(int n = 0; n < 22; n++) { w->reveal[n] = false; }
    //Caracteres de espaço semppre são previamente revelados
    for(int n = 0; n < strlen(w->string); n++) { if(w->string[n] == 32) { w->reveal[n] = true; } } //32 == ' '
    w->helps[1] = w->helps[0];
}
void Word_restoreAll(Word wv[], int len) {                              //Oculta os caracteres de todas as palavras e retaura as ajudas
    for(int i = 0; i < len; i++) {
        Word_restoreOne(&wv[i]);
    }
}
void Word_restoreByIndex(Word wv[], int len, int idx[], int e) {        //Oculta os caracteres das palavras via índice e retaura as dicas
    for(int i = 0; i < e; i++) {
        if(idx[i]>0 && idx[i]<len) { Word_restoreOne(&wv[idx[i]]); }
    }
}

typedef struct {    //Define o tipo Gallow (Forca)
    Vector2 pos;        //Posição da forca
    int middleSize;     //Tamanho da barra central
    int gap;            //Espaçamento
    int endSize;        //Tamanho das barras secundárias
    int segments;       //Número de segmentos
    int thickness;      //Expessura da linha
    Color color;        //Cor da forca
} Gallow;
void Gallow_init(Gallow* g, Vector2 p, int ms, int gp, int es, int sg, int tn, Color c) {   //Função de iniciação da forca
    g->pos = p;
    g->middleSize = ms;
    g->gap = gp;
    g->endSize = es;
    g->segments = sg;
    g->thickness = tn;
    g->color = c;
}
void Gallow_centerX(Gallow* g, int sw) { g->pos.x = sw/2 - g->middleSize/2; }               //Centralizar a forca no centro da tela
void Gallow_show(Gallow g) {                                                                //Mostra a forca
    DrawRectangle(g.pos.x-g.middleSize/2,g.pos.y,g.middleSize,g.thickness,g.color);
    for(int r = 0; r < g.segments; r++) {
        DrawRectangle(g.pos.x+(g.middleSize/2)+g.gap*(r+1)+g.endSize*r, g.pos.y, g.endSize, g.thickness, g.color);
        DrawRectangle(g.pos.x-(g.middleSize/2)-g.gap*(r+1)-g.endSize*r-g.endSize, g.pos.y, g.endSize, g.thickness, g.color);
    }
    DrawRectangle(g.pos.x-g.thickness/2, g.pos.y+g.thickness, g.thickness, g.endSize*2, g.color);
}
void Gallow_updateColor(Gallow* g, Style s) { g->color = s.gallow; }                        //Atualiza a cor da forca

typedef struct {    //Define o tipo Hangman (Enforcado)
    Vector2 headPos;    //Posição da cabeça (referência)    
    float headRadius;   //Raio  da cabeça (referência)
    int thickness;      //Expessura da linha
    int resolution;     //Resolução
    float scale;        //Escala do personagem
    int damage;         //Dano ao personagem (erros)
    int dieIn;          //Limite de dano (6)
    Color color;        //Cor do enforcado
} Hangman;
void Hangman_init(Hangman* h, Vector2 hp, float hr, int t, int r, float s, Color c) {   //Função de iniciação do enforcado
    h->headPos = hp;
    h->headRadius = hr;
    h->thickness = t;
    h->resolution = r;
    h->scale = s;
    h->color = c;
    h->damage = 0;
    h->dieIn = 6;
}
void Hangman_show(Hangman h) {                                                          //Mostrar o  enforcado
    if(h.damage >= 1) { //Head
        DrawRing(
            h.headPos,
            h.headRadius*h.scale,
            h.headRadius-h.thickness,
            0.0f,
            360.0f,
            h.resolution,
            h.color
        );
    }
    if(h.damage >= 2) { //Body
        DrawRectangle(
            h.headPos.x-(h.thickness/2),
            h.headPos.y+h.headRadius,
            h.thickness,
            h.headRadius*2.6*h.scale,
            h.color
        );
    }
    if(h.damage >= 3) { //Left Arm
        DrawLineEx(
            (Vector2){h.headPos.x-(h.thickness/4)*h.scale,h.headPos.y+h.headRadius*1.2*h.scale},
            (Vector2){(h.headPos.x-(h.thickness/4))*0.92*h.scale,(h.headPos.y+h.headRadius*1.2)*1.25*h.scale},
            h.thickness,
            h.color
        );
    }
    if(h.damage >= 4) { //Right Arm
        DrawLineEx(
            (Vector2){h.headPos.x+(h.thickness/4)*h.scale,h.headPos.y+h.headRadius*1.2*h.scale},
            (Vector2){(h.headPos.x+(h.thickness/4))*1.08*h.scale,(h.headPos.y+h.headRadius*1.2)*1.25*h.scale},
            h.thickness,
            h.color
        );
    }
    if(h.damage >= 5) { //Left Leg
        DrawLineEx(
            (Vector2){h.headPos.x+(h.thickness/4)*h.scale,h.headPos.y+h.headRadius-(h.thickness/4)+h.headRadius*2.6*h.scale},
            (Vector2){(h.headPos.x+(h.thickness/4))*0.92*h.scale,(h.headPos.y+h.headRadius+h.headRadius*2.6)*1.18*h.scale},
            h.thickness,
            h.color
        );
    }
    if(h.damage >= 6) { //Right Leg
        DrawLineEx(
            (Vector2){h.headPos.x+(h.thickness/4)*h.scale,h.headPos.y+h.headRadius-(h.thickness/4)+h.headRadius*2.6*h.scale},
            (Vector2){(h.headPos.x+(h.thickness/4))*1.08*h.scale,(h.headPos.y+h.headRadius+h.headRadius*2.6)*1.18*h.scale},
            h.thickness,
            h.color
        );
    }
}
void Hangman_getDamage(Hangman* h, int d) { h->damage += d; }                           //Função para inserir dano
bool Hangman_isDead(Hangman h) { return h.damage >= h.dieIn; }                          //Verificar se o enforcado atigiu o limite de dano
void Hangman_revive(Hangman* h) { h->damage = 0; }                                      //Restaura o dano para 0
void Hangman_updateColor(Hangman* h, Style s) { h->color = s.hangman; }                 //Atualiza a cor do enforcado

typedef struct { //Define o tipo LetterList (Tabela de letras)
    Text letters;           //Letras da tabela
    Style marker;           //Estilo adotado
    Vector2 cursor;         //Posição do cursor
    int lines;              //Quantidade de linhas na tabela
    bool rightIndex[27];    //Letras coorretas
    bool wrongIndex[27];    //Letras incorretas
} LetterList;
void LetterList_init(LetterList* ll, Text t, Style s, int ln) {             //Função de iniciação do enforcado
    ll->letters = t;
    ll->marker = s;
    ll->cursor = (Vector2){0,0};
    ll->lines = ln;
    for(int n = 0; n < 27; n++) { ll->rightIndex[n] = false; }
    for(int n = 0; n < 27; n++) { ll->wrongIndex[n] = false; }
}
void LetterList_charsShow(LetterList ll) {                                  //Mostrar as letras
    int space = 0;          //Espaçamento horizontal
    int intercalate = -1;   //Espaçamento vertical
    for(int i = 0; i < 27; i++) {
        if(intercalate + 1 >= ll.lines) { intercalate = 0; space++; } else { intercalate++; }
        int state = 0;
        Color cl;   //Cor da letra
        
        (ll.rightIndex[i]) ? state = 1 : state; //Altera o estado para a letra usada corretamente
        (ll.wrongIndex[i]) ? state = 2 : state; //Altera o estado para a letra usada erroneamente

        if(state == 0) { cl = ll.marker.commonText; }
        if(state == 1) { cl = ll.marker.gallow; }
        if(state == 2) { cl = ll.marker.disableText; }
        DrawTextEx(     //Desenha a letra
            ll.letters.font,
            (ll.letters.string[i] == 'c') ? "Ç" : TextFormat("%c",ll.letters.string[i]),    //TextFormat não tem suporte para 'Ç'
            (Vector2){
                ll.letters.pos.x+((ll.letters.fontSize+ll.letters.fontSize/10)*space*1.5)-(ll.letters.textSize.x/2),
                ll.letters.pos.y+(ll.letters.textSize.y*2.5*intercalate),
            },
            ll.letters.fontSize,
            ll.letters.fontSize/10,
            cl
        );
        /*i == 27
          ? DrawTextEx(ll.letters.font," ",(Vector2){0,0},0,0,(Color){0,0,0,0})
          : DrawTextEx(
                ll.letters.font,
                sep,
                (Vector2){
                    ll.letters.pos.x+((ll.letters.fontSize+ll.letters.fontSize/10)*space),
                    ll.letters.pos.y,
                },
                ll.letters.fontSize,
                ll.letters.fontSize/10,
                ll.marker.commonText
            );*/
    }
}
void LetterList_cursorShow(LetterList ll) {                                 //Mostra o cursor
    int space = 0;
    int intercalate = -1;
    for(int i = 0; i < 27; i++) {
        if(intercalate+1>=ll.lines) { intercalate = 0; space++; } else { intercalate++; }
        if(ll.cursor.x == space && ll.cursor.y == intercalate) {
            DrawRectangleV(
                (Vector2){
                    ll.letters.pos.x+((ll.letters.fontSize+ll.letters.fontSize/10)*space*1.5)-(ll.letters.textSize.x/2)-ll.letters.fontSize/5,
                    ll.letters.pos.y+(ll.letters.textSize.y*2.5*intercalate)-ll.letters.fontSize/12,
                },
                (Vector2){
                    ll.letters.fontSize,
                    ll.letters.fontSize,
                },
                Fade(ll.marker.activeText,0.7)
            );
        }
    }
}
void LetterList_cursorMove(LetterList* ll, Vector2 m, bool r) {             //Movimenta o cursor dentro do espaço permitido
    //r == true ? Move o cursor : Incrementa no cursor
    int slen = strlen(ll->letters.string);
    int lnlen = slen / ll->lines - 1;
    if(r) { ll->cursor = m; }
    else  { ll->cursor.x += m.x; ll->cursor.y += m.y; }
    //Comportamento ciclico do cursor
    (ll->cursor.x < 0) ? ll->cursor.x = lnlen : 0;
    (ll->cursor.x > lnlen) ? ll->cursor.x = 0 : 0;
    (ll->cursor.y < 0) ? ll->cursor.y = ll->lines-1 : 0;
    (ll->cursor.y > ll->lines-1) ? ll->cursor.y = 0 : 0;
}
void LetterList_updateColor(LetterList* ll, Style m) { ll->marker = m; }    //Atualiza o tema do marcador
int  LetterList_canChoose(LetterList ll) {                                  //Verifica se a letra pode ser escolhida
    int retErr = 0;
    int currChar = ll.cursor.y + (ll.cursor.x * ll.lines);
    int vdtLen = sizeof(ll.rightIndex)/sizeof(ll.rightIndex[0]);
    if(vdtLen < currChar || 0 > currChar) { retErr = 2; }
    else if(ll.rightIndex[currChar] || ll.wrongIndex[currChar]) { retErr = 1; }
    else { retErr = 0; }
    return retErr;
}
void LetterList_restore(LetterList* ll) {                                   //Restaura a lista para o estado inicial
    for(int n = 0; n < 27; n++) { ll->rightIndex[n] = false; }
    for(int n = 0; n < 27; n++) { ll->wrongIndex[n] = false; }
}
bool LetterList_choose(LetterList* ll, Word* w) {                           //Seleciona uma letra da lista, verifica e altera na palavra
    bool have = false;
    int currChar = ll->cursor.y + (ll->cursor.x * ll->lines);
    for(int i = 0; i < w->lenght; i++) {
        if(ll->letters.string[currChar] == w->string[i]) {
            have = true;
            ll->rightIndex[currChar] = true;
            w->reveal[i] = true;
        }
    }
    if(!have) { ll->wrongIndex[currChar] = true; }
    return have;
}
void Word_autoGuess(Word* w, LetterList* ll) {                              //Revela aleatoriamente uma letra oculta da palavra, remove da lista e consome uma ajuda
    int idx;
    while(true) {
        idx = rand() % (w->lenght + 1);
        if(!w->reveal[idx]) { break; }
    }
    for(int i = 0; i < w->lenght; i++) {
        if(w->string[i] == w->string[idx]) {
            w->reveal[i] = true;
        }
    }
    int riLen = sizeof(ll->rightIndex)/sizeof(ll->rightIndex[0]);
    for(int i = 0; i < riLen; i++) {
        if(ll->letters.string[i] == w->string[idx]) { ll->rightIndex[i] = true; }
    }
    w->helps[1]--;
}


int main() {
    int gameState         = 1;                 //Tela do jogo
    int selectMain        = 1;                 //Opção de seleção no menu de jogo (1~3)
    int selectOptn        = 4;                 //Opção de seleção nas opções (4~6)
    int selectStyle       = 0;                 //Opção de seleção do estilo (0~LEN_STYLE)
    int selectWord        = 0;                 //Seleção aleatória da palavra do jogo (0~LEN_WORDS)
    int pickedWord[3];                         //Índice das palavras escolhidas 
    int selectEndLevel    = 7;                 //Opção de seleção no  final da rodada (7~8)
    bool fpsOnScreen      = false;             //Opção de seleção da taxa de FPS
    int setFps            = 60;                //FPS
    bool updateFps        = true;              //Atualizar taxa de FPS
    int incCursorX        = 0;                 //Incremento do cursor para X
    int incCursorY        = 0;                 //Incremento do cursor para Y
    bool begin            = true;              //Começar nova partida
    int level             = 0;                 //Nível da partida
    const int levelDif[3] = {4,9,14};          //Limites de seleção. Fácil == [0,levelDif[0]], Médio == [levelDif[0]+1,levelDif[1]], Difícil == [levelDif[1]+1,levelDif[2]]
    bool hint             = false;             //Mostrar dica
    bool end              = false;             //Partida encerradda
    bool win              = false;             //Partida ganha
    bool loose            = false;             //Partida perdida
    bool reset            = false;             //Reiniciar ppartida desde o começo
    const Font regFont    = GetFontDefault();  //Fonte do texto do jogo
    

    //Vetor de todos os temas do jogo e definição de todos os temas
    Style styles[LEN_STYLE];
    Style_init(
        &styles[0],
        "DEEP PURPLE",
        (Color){32,28,55,255},
        (Color){248,248,242,255},
        (Color){73,255,81,255},
        (Color){171,119,255,255},
        (Color){54,48,86,255},
        (Color){189,112,39,255},
        (Color){97,234,217,255}
    );
    Style_init(
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
    Style_init(
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
    Style_init(
        &styles[3],
        "FOREST",
        (Color){18,30,28,255},
        (Color){248,248,242,255},
        (Color){238,196,94,255},
        (Color){73,231,208,255},
        (Color){30,50,47,255},
        (Color){138,217,255,255},
        (Color){88,151,89,255}
    );
    Style_init(
        &styles[4],
        "DEEP SEA",
        (Color){21,23,60,255},
        (Color){88,183,255,255},
        (Color){225,49,116,255},
        (Color){255,246,179,255},
        (Color){48,51,100,255},
        (Color){134,219,253,255},
        (Color){189,132,221,255}
    );
    Style_init(
        &styles[5],
        "VULCANO",
        (Color){16,0,0,255},
        (Color){238,237,232,255},
        (Color){124,23,31,255},
        (Color){192,71,80,255},
        (Color){147,108,107,255},
        (Color){92,140,151,255},
        (Color){39,192,180,255}
    );
    Style_init(
        &styles[6],
        "SNOW",
        (Color){255,255,255,255},
        (Color){77,77,74,255},
        (Color){17,54,185,255},
        (Color){43,159,230,255},
        (Color){229,229,238,255},
        (Color){25,169,143,255},
        (Color){121,7,76,255}
    ); 
    Style_init(
        &styles[7],
        "NIGHTFALL",
        (Color){0,0,0,255},
        (Color){241,236,242,255},
        (Color){79,82,135,255},
        (Color){123,124,163,255},
        (Color){144,164,184,255},
        (Color){25,118,239,255},
        (Color){76,148,214,255}
    );
    Style_init(
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
    Style_init(
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
    
    
    //Variável da forca e inicialização
    Gallow gallow;
    Gallow_init(&gallow,(Vector2){SCR_W/2,50},220,10,30,6,6,styles[selectStyle].gallow);
    
    
    //Variável do enforcado e inicialização
    Hangman hangman;
    Hangman_init(
        &hangman,
        (Vector2){
            SCR_W/2,
            (gallow.pos.y)+(gallow.endSize*2)+32 //32 -> headRadius
        },
        32,
        6,
        40,
        1,
        styles[selectStyle].hangman
    );
    
    
    //Variável da lista e inicialização
    LetterList list;
    LetterList_init(
        &list,
        (Text){
            -1,
            "ABCcDEFGHIJKLMNOPQRSTUVWXYZ",
            regFont,
            28,
            (Vector2){386,19}, //change
            (Vector2){SCR_W/2,460},
            0,
            (Color){0,0,0,0}
        },
        styles[selectStyle],
        3
    );
    
    //Variáveis de texto e suas inicializações
    Text titleText; //Título no menu
    Text_init(&titleText,0,"Jogo da Forca",regFont,52,(Vector2){0,10},(Vector2){385,47},1);
    Text_centerX(&titleText,SCR_W,(double)1/2);
    
        Text playText;      //Opção de jogar no menu /*Texto diferent para quando o jogo já estiver iniciado*/
        Text_init(&playText,1,"Jogar",regFont,40,(Vector2){20,500},(Vector2){116,36},3);
        Text_centerX(&playText,SCR_W,(double)1/6);
    
        Text optionText;    //Opção de configuração no menu
        Text_init(&optionText,2,"Opçoes",regFont,40,(Vector2){220,500},(Vector2){144,36},3);
        Text_centerX(&optionText,SCR_W,(double)1/2);
        
        Text exitText;      //Opção de sair no menu
        Text_init(&exitText,3,"Sair",regFont,40,(Vector2){420,500},(Vector2){80,28},3);
        Text_centerX(&exitText,SCR_W,(double)5/6);
        
        Text creditText;    //Crédito
        Text_init(&creditText,-1,"by: Leonardo Kenji Ueze & Marco Antonio Zerbielli Bee",regFont,14,(Vector2){7,580},(Vector2){391,13},4);
    
    Text optionTitleText;   //Título nas opções
    Text_init(&optionTitleText,0,"Opçoes",regFont,40,(Vector2){0,10},(Vector2){144,36},1);
    Text_centerX(&optionTitleText,SCR_W,(double)1/2);
    
        Text themeText;     //Opção de tema
        Text_init(&themeText,4,"Tema: ",regFont,28,(Vector2){10,80},(Vector2){92,19},3);
        
        Text turnFpsText;   //Opção de FPS
        Text_init(&turnFpsText,5,"Mostrar FPS: ",regFont,28,(Vector2){10,120},(Vector2){209,19},3);
        
        Text setFpsText;    //Opção de definir FPS
        Text_init(&setFpsText,6,"Definir FPS: ",regFont,28,(Vector2){10,160},(Vector2){209,19},3);

    Text winLevelText;      //Texto de vitória
    Text_init(&winLevelText,0,"VITÓRIA! Continuar?",regFont,34,(Vector2){0,450},(Vector2){364,24},1);
    Text_centerX(&winLevelText,SCR_W,(double)1/2);    
    
    Text looseLevelText;    //Texto de derrota
    Text_init(&looseLevelText,0,"DERROTA! Continuar?",regFont,34,(Vector2){40,450},(Vector2){381,24},1);
    Text_centerX(&looseLevelText,SCR_W,(double)1/2);
        
        Text resetAlertText;    //Opção de reiniciar o jogo
        Text_init(&resetAlertText,9,"> (0) Reiniciar",regFont,20,(Vector2){20,80},(Vector2){136,14},3);
        
        Text hintAlertText;     //Opção de dica do jogo
        Text_init(&hintAlertText,10,"> (1) Dica",regFont,20,(Vector2){20,110},(Vector2){86,14},3);
        
        Text helpAlertText;     //Opção de ajuda no jogo
        Text_init(&helpAlertText,11,"> (2) Ajudas: ",regFont,20,(Vector2){20,140},(Vector2){134,14},3);
        
        Text advanceText;       //Opção de avançar nível
        Text_init(&advanceText,7,"Sim",regFont,26,(Vector2){20,530},(Vector2){36,188},3);
        Text_centerX(&advanceText,SCR_W,(double)0.3);
        
        Text backText;          //Opção de desistir da partida /*win =>*/
        Text_init(&backText,8,"Não",regFont,26,(Vector2){120,530},(Vector2){46,18},3);
        Text_centerX(&backText,SCR_W,(double)0.7);
    
    Text endText;               //Texto de fim de jogo
    Text_init(&endText,0,"Fim de Jogo",regFont,48,(Vector2){0,10},(Vector2){278,33},1);
    Text_centerX(&endText,SCR_W,(double)1/2);
    
        Text endSubText;        //Instrução de fim de jogo
        Text_init(&endSubText,0,"\"Pressione Backspace para voltar\"",regFont,24,(Vector2){0,370},(Vector2){438,17},1);
        Text_centerX(&endSubText,SCR_W,(double)1/2);
    
    
    //Vetor do endereço dos textos
    Text* texts[LEN_TEXTS] = {
        &titleText,&playText,&optionText,
        &exitText,&creditText,&optionTitleText,
        &themeText,&turnFpsText,&setFpsText,
        &winLevelText,&looseLevelText,&advanceText,
        &backText, &endText, &endSubText,
        &resetAlertText, &hintAlertText, &helpAlertText
    };
    
    Word words[LEN_WORDS];  //Vetor das palavras
    //FÁCIL
    Word_init(&words[0],"CAIXA","Objeto de armazenagem",238,2);
    Word_init(&words[1],"ESCALIMETRO","Instrumento de medição",238,0);
    Word_init(&words[2],"CHAVE DE RODA","Ferramenta automotiva",238,0);
    Word_init(&words[3],"MACACO","Mamífero",90,0);
    Word_init(&words[4],"DIFICIL","A palavra é difícil",180,0);
    //MÉDIO
    Word_init(&words[5],"GENGIVITE","Doença Inflamatória",202,1);
    Word_init(&words[6],"PALINDROMO","Propriedade da Lingua Portuguesa",356,1);
    Word_init(&words[7],"HEMOFILIA","Sangue",72,1);
    Word_init(&words[8],"MAGNITUDE","Terremoto",108,1);
    Word_init(&words[9],"CONCATENAR","Junção",70,1);
    //DIFICIL
    Word_init(&words[10],"ANACOLUTO","Recurso estilístico",192,2);
    Word_init(&words[11],"ALMANAQUE","Mídia física",111,2);
    Word_init(&words[12],"INSIGNIFICANCIA","Qualidade ou estado",204,2);
    Word_init(&words[13],"OCEANODROMO","Categoria de peixe",190,2);
    Word_init(&words[14],"BEQUILHA","Peça de avião",140,2);
    
    InitWindow(SCR_W, SCR_H, "Jogo da forca");  //Inicia a GUI
    
    //Laço de repetição do jogo. Encera ao teclar ESC, fechar a guia ou selecionar sair no menu
    while(!WindowShouldClose() && gameState != 0) {
        if(updateFps) { SetTargetFPS(setFps); updateFps = false; }  //Atualiza o FPS
        Text_updateColor(texts,styles[selectStyle],LEN_TEXTS);      //Atualiza as cores dos textos
        BeginDrawing();                                             //Inicia a fase de desenho na GUI
            ClearBackground(styles[selectStyle].bg);                //Limpa a tela
            bool keyUp    = IsKeyPressed(KEY_UP);                   //Estado da tecla de seta "cima"
            bool keyDown  = IsKeyPressed(KEY_DOWN);                 //Estado da tecla de seta "baixo"
            bool keyLeft  = IsKeyPressed(KEY_LEFT);                 //Estado da tecla de seta "esquerda"
            bool keyRight = IsKeyPressed(KEY_RIGHT);                //Estado da tecla de seta "direita"
            bool keyRetrn = IsKeyPressed(KEY_ENTER);                //Estado da tecla return (enter)
            bool keyBcksp = IsKeyPressed(KEY_BACKSPACE);            //Estado da tecla backspace
            bool keyNum0  = IsKeyPressed('0');                      //Estado da tecla '0'
            bool keyNum1  = IsKeyPressed('1');                      //Estado da tecla '1'
            bool keyNum2  = IsKeyPressed('2');                      //Estado da tecla '2'
            
            if(keyBcksp)       { gameState = 1; }                   //Backspace volta ara o menu
            if(fpsOnScreen)    { DrawFPS(10,10); }                  //Mostra o  FPS na tela
            switch(gameState)  {                                    //Condições da tela de jogo
                //1 == Menu; 2 == Jogo; 3 == Opções; 4 == Tela de vitória
                case(1):
                    //Mostra os textos do menu
                    Text_show(titleText);
                    Text_show(playText);
                    Text_show(optionText);
                    Text_show(exitText);
                    Text_show(creditText);
                    
                    //Seleção do menu
                    if(keyRight) { selectMain++; }
                    if(keyLeft)  { selectMain--; }
                    if(keyRetrn) {
                        switch(selectMain) {
                            case(1): gameState = 2; break;
                            case(2): gameState = 3; break;
                            case(3): gameState = 0; break;
                            default: break;
                        }
                    }
                    //Ciclo do menu
                    selectMain < 1 ? selectMain = 3 : selectMain;
                    selectMain > 3 ? selectMain = 1 : selectMain;
                    
                    //Ativação de textos
                    Text_activate(&playText,   selectMain);
                    Text_activate(&optionText, selectMain);
                    Text_activate(&exitText,   selectMain);
                    break;
                case(2):
                    //Mostra a extrutura da forca
                    Gallow_show(gallow);
                    Hangman_show(hangman);
                    
                    //Reinicia a partida
                    if(begin) {
                        begin = false;
                        int lower = level != 0 ? levelDif[level-1]+1 : 0;
                        int upper = levelDif[level];
                        selectWord = rand() % (upper - lower + 1) + lower;  /*Introduzir algoritmo para evitar repetições*/
                        pickedWord[level] = selectWord;
                    }
                    
                    //Ativar reinicio a partida
                    if(keyNum0)                 { reset = !reset; }
                    if(keyRetrn && reset)       { gameState = 4; break; }
                    
                    //Indicar o movimento do cursor na lista
                    if(keyUp && !end && !reset)    { incCursorY--; }
                    if(keyDown && !end && !reset)  { incCursorY++; }
                    if(keyRight && !end && !reset) { incCursorX++; }
                    if(keyLeft && !end && !reset)  { incCursorX--; }
                    
                    //Movimentação do cursor
                    if(incCursorX != 0 || incCursorY != 0) { LetterList_cursorMove(&list,(Vector2){incCursorX,incCursorY},false); }
                    incCursorX = 0;
                    incCursorY = 0;
                    
                    //Escolher letra da lista
                    if(keyRetrn && !end && !reset) {
                        if(LetterList_canChoose(list) == 0) {   //Verificar se a condição de escolha é possível
                            Hangman_getDamage(&hangman, !LetterList_choose(&list,&words[selectWord]) ? 1 : 0); //Insere o dano (se existir)
                        }
                        win = Word_isComplete(words[selectWord]);   //Verifica a vitória
                        loose = Hangman_isDead(hangman);            //Verifica a derrota
                    }
                    
                    //Selecionar opção após o fim da partida
                    if(keyRight && end && !reset) { selectEndLevel++; }
                    if(keyLeft && end && !reset)  { selectEndLevel--; }
                    if(keyRetrn && end && !reset) {
                        //Avanço / desistência
                        if(selectEndLevel == 7 && win) { level++; }
                        if(selectEndLevel == 8 && win) { level = 0; }
                        //Atualização de variáveis para as condições da próxima fase
                        begin = true;
                        win = false;
                        loose = false;
                        end = false;
                        hint = false;
                        selectEndLevel = 7;
                        Hangman_revive(&hangman);
                        Word_restoreOne(&words[selectWord]);
                        LetterList_restore(&list);
                        LetterList_cursorMove(&list,(Vector2){0,0},true);
                        if(level >= 3) { gameState = 4; break; }   //Fim de jogo
                    }
                    //Ciclo da seleção pós-vitória
                    (selectEndLevel > 8) ? selectEndLevel = 7 : selectEndLevel;
                    (selectEndLevel < 7) ? selectEndLevel = 8 : selectEndLevel;
                    
                    //Ativação e apresentação da dica
                    if(keyNum1) { hint = !hint; }
                    if(hint) {
                        DrawTextEx(
                            regFont,
                            words[selectWord].tip,
                            (Vector2){
                                SCR_W/2 - words[selectWord].tipSize/2,
                                20
                            },
                            20,
                            20/10,
                            styles[selectStyle].commonText
                        );
                    }
                    
                    //Ajuda
                    if(keyNum2 && Word_haveHelps(words[selectWord])) { Word_autoGuess(&words[selectWord],&list); }
                    
                    end = win || loose; //Verificação de fim de partida
                    
                    //Ativar o texto de reset e adicionar indicativo
                    Text_activate(&resetAlertText, reset ? 9 : 0);
                    if(reset) { Text_cpFormatAndShow(resetAlertText, " (Enter)"); }

                    //Informações que serão mostradas na tela e atualizadas de acordo com o estado da partida
                    if(win)        { Text_show(winLevelText); }
                    else if(loose) { Text_show(looseLevelText); }
                    else {
                        Text_activate(&hintAlertText, hint ? 10 : 0);
                        Text_activate(&helpAlertText, hint ? 10 : 0);
                        Text_disable(&helpAlertText,  words[selectWord].helps[1] <= 0 ? 11 : 0);
                        if(!reset) { LetterList_cursorShow(list); }
                        LetterList_charsShow(list);
                        Text_show(hintAlertText);
                        Text_show(helpAlertText);
                        Text_cpFormatAndShow(helpAlertText, TextFormat("%d/%d", words[selectWord].helps[1], words[selectWord].helps[0]));
                    }
                    Text_show(resetAlertText);
                    Word_showWord(words[selectWord],(Vector2){SCR_W/2,390},regFont,styles[selectStyle],end);
                    Word_showUnderline(words[selectWord],(Vector2){SCR_W/2,400},styles[selectStyle].gallow);
                    
                    //Se a partida acabar, substitui a lista pelo quadro de avançar / desistir
                    if(end) {
                        Text_activate(&advanceText, !reset ? selectEndLevel : 0);
                        Text_activate(&backText, !reset ? selectEndLevel : 0);
                        Text_show(advanceText);
                        Text_show(backText);
                    }
                    break;
                case(3):
                    //Mostra os textos
                    Text_show(optionTitleText);
                    Text_show(themeText);
                    Text_cpFormatAndShow(themeText,styles[selectStyle].name);
                    Text_show(turnFpsText);
                    Text_cpFormatAndShow(turnFpsText,TextFormat("%s",fpsOnScreen?"SIM":"NÃO"));
                    Text_show(setFpsText);
                    Text_cpFormatAndShow(setFpsText,TextFormat("%d",setFps));
                    
                    //Variáveis booleanas das opções
                    bool isTheme   = selectOptn == 4;
                    bool isTurnFps = selectOptn == 5;
                    bool isSetFps  = selectOptn == 6;
                    
                    //Mover seleção entre as opções
                    if(keyDown) { selectOptn++; }
                    if(keyUp)   { selectOptn--; }
                    
                    //Movificar a opção de tema
                    if(keyRight && isTheme) { selectStyle++; }
                    if(keyLeft && isTheme)  { selectStyle--; }
                    
                    //Movificar a opção de ativar FPS
                    if((keyLeft || keyRight) && isTurnFps) { fpsOnScreen = !fpsOnScreen; }
                    
                    //Movificar o FPS
                    if(keyRight && isSetFps) { setFps += 10; updateFps = true; }
                    if(keyLeft && isSetFps)  { setFps -= 10; updateFps = true; }
                    
                    //Opções cíclicas
                    selectOptn < 4 ? selectOptn = 6 : selectOptn;
                    selectOptn > 6 ? selectOptn = 4 : selectOptn;
                    selectStyle < 0 ? selectStyle = 9 : selectStyle;
                    selectStyle > 9 ? selectStyle = 0 : selectStyle;
                    setFps > 120 ? setFps = 30 : setFps;
                    setFps < 30 ? setFps = 120 : setFps;
                    
                    //Ativar os textos
                    Text_activate(&themeText, selectOptn);
                    Text_activate(&turnFpsText, selectOptn);
                    Text_activate(&setFpsText, selectOptn);
                    
                    //Atualizar o tema da forca, do enforcado e da lista
                    Gallow_updateColor(&gallow,styles[selectStyle]);
                    Hangman_updateColor(&hangman,styles[selectStyle]);
                    LetterList_updateColor(&list,styles[selectStyle]);
                    break;
                case(4):
                    //Mostrar os textos da tela de fim de jogo
                    Text_show(endText);
                    Text_show(endSubText);
                    
                    //Atualiza as variáveis necessárias para recomeçar um novo jogo
                    selectMain = 1;
                    selectOptn = 4;
                    selectWord = 0;
                    selectEndLevel = 7;
                    incCursorX = 0;
                    incCursorY = 0;
                    begin = true;
                    level = 0;
                    hint = false;
                    end = false;
                    win = false;
                    loose = false;
                    reset = false;
                    
                    //Reinicia as palavras usadas e a lista
                    Word_restoreByIndex(words, LEN_WORDS, pickedWord, sizeof(pickedWord)/sizeof(int));
                    LetterList_restore(&list);
                    LetterList_cursorMove(&list,(Vector2){0,0},true);
                    break;
                default: gameState = 0; //Padrão, fechar jogo
            }
        EndDrawing();   //Finaliza a atividade de desenho
    }
    CloseWindow();      //Fecha a guia
    return 0;
}
