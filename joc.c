#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


#define WIN_H 20
#define WIN_W 57

#define BRICK_ROWS 5
#define BRICK_COLS 9
#define BRICK_W 5
#define BRICK_START_Y 1
#define BRICK_START_X 2

#define PADDLE_W 11
#define PADDLE_Y (WIN_H - 2)
#define PADDLE_SPEED 6

#define BALL_SPEED_Y 0.12

typedef struct {
    int x, y;
    int on; //dc a fost sparta sau nu
    int color;
    int points;
} Brick;

Brick bricks[BRICK_ROWS][BRICK_COLS];
int paddle_x, old_paddle_x;
double ball_x, ball_y;
int old_ball_x, old_ball_y, ok;
double ball_dx, ball_dy;

int scor = 0;
int bricks_ramase = 0;
int joc_lansat = 0;
int stare_finala = 0; // 0 dc e in desfasurare, 1 dc a castigat, 2 dc a pierdut

void init_joc() {
    scor = 0;
    bricks_ramase = BRICK_ROWS * BRICK_COLS;
    joc_lansat = 0;
    stare_finala = 0;

    paddle_x = (WIN_W - PADDLE_W) / 2; //mingea e plasata in centrul paletei, deasupra ei
    old_paddle_x = paddle_x;

    ball_x = paddle_x + PADDLE_W / 2;
    ball_y = PADDLE_Y - 1;
    old_ball_x = (int)ball_x; //fctiile din ncurses folosesc nr intregi la coord
    old_ball_y = (int)ball_y;

    ball_dx = (rand() % 2 == 0) ? 0.12 : -0.12; //alegem random pozitia orizontala
    ball_dy = -BALL_SPEED_Y; //mingea pleaca in sus

    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].on = 1;
            bricks[i][j].y = BRICK_START_Y + i;
            bricks[i][j].x = BRICK_START_X + j * (BRICK_W + 1); // calc pozitia fiecarei bricks ai intre ele sa fie spatiu de 1 ch
            bricks[i][j].color = i + 1;
            bricks[i][j].points = (BRICK_ROWS - i) * 10;
        }
    }
}

void deseneaza_chenar_model(WINDOW *win) {
    for (int x = 1; x < WIN_W - 1; x++) {
        if (x % 2 == 0) {
            wattron(win, COLOR_PAIR(5) | A_BOLD); //activeaza atributele
            mvwaddch(win, 0, x, ACS_CKBOARD); // afiseaza caracter - dreptunghiul albastru
            mvwaddch(win, WIN_H - 1, x, ACS_CKBOARD);
            wattroff(win, COLOR_PAIR(5) | A_BOLD); //dezactiveaza
        } else {
            wattron(win, COLOR_PAIR(6) | A_BOLD);
            mvwaddch(win, 0, x, ACS_HLINE);
            mvwaddch(win, WIN_H - 1, x, ACS_HLINE);
            wattroff(win, COLOR_PAIR(6) | A_BOLD);
        }
    }

    for (int y = 1; y < WIN_H - 1; y++) {
        if (y % 2 == 0) {
            wattron(win, COLOR_PAIR(5) | A_BOLD);
            mvwaddch(win, y, 0, ACS_CKBOARD);
            mvwaddch(win, y, WIN_W - 1, ACS_CKBOARD);
            wattroff(win, COLOR_PAIR(5) | A_BOLD);
        } else {
            wattron(win, COLOR_PAIR(6) | A_BOLD);
            mvwaddch(win, y, 0, ACS_VLINE);
            mvwaddch(win, y, WIN_W - 1, ACS_VLINE);
            wattroff(win, COLOR_PAIR(6) | A_BOLD);
        }
    }
 // in colturi avem diamant
    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwaddch(win, 0, 0, ACS_DIAMOND);
    mvwaddch(win, 0, WIN_W - 1, ACS_DIAMOND);
    mvwaddch(win, WIN_H - 1, 0, ACS_DIAMOND);
    mvwaddch(win, WIN_H - 1, WIN_W - 1, ACS_DIAMOND);

    mvwaddch(win, 0, 3, ACS_RTEE);
    mvwprintw(win, 0, 4, " BREAKOUT ");
    mvwaddch(win, 0, 14, ACS_LTEE);

    mvwaddch(win, 0, WIN_W - 18, ACS_RTEE);
    mvwprintw(win, 0, WIN_W - 17, " Scor: %04d ", scor);
    mvwaddch(win, 0, WIN_W - 5, ACS_LTEE);
    wattroff(win, COLOR_PAIR(3) | A_BOLD);
}

void deseneaza_fereastra(WINDOW *win) {
    mvwaddch(win, old_ball_y, old_ball_x, ' '); //stergem mingea si paleta din poz anterioare
    for (int k = 0; k < PADDLE_W; k++) {
        mvwaddch(win, PADDLE_Y, old_paddle_x + k, ' ');
    }

    deseneaza_chenar_model(win);

    //desenam caramizile
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            if (bricks[i][j].on) {
                wattron(win, COLOR_PAIR(bricks[i][j].color) | A_BOLD);
                for (int k = 0; k < BRICK_W; k++) {
                    mvwaddch(win, bricks[i][j].y, bricks[i][j].x + k, ACS_CKBOARD); // o caramida e lata de 5 ch
                }
                wattroff(win, COLOR_PAIR(bricks[i][j].color) | A_BOLD);
            } else {
                for (int k = 0; k < BRICK_W; k++) {
                    mvwaddch(win, bricks[i][j].y, bricks[i][j].x + k, ' '); // dc e sparta o stergem
                }
            }
        }
    }
    // desenam paleta
    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwaddch(win, PADDLE_Y, paddle_x, '<');
    mvwaddch(win, PADDLE_Y, paddle_x + PADDLE_W - 1, '>');
    wattroff(win, COLOR_PAIR(3) | A_BOLD);

    wattron(win, COLOR_PAIR(6) | A_BOLD);
    for (int k = 1; k < PADDLE_W - 1; k++) {
        mvwaddch(win, PADDLE_Y, paddle_x + k, '=');
    } // <========>
    wattroff(win, COLOR_PAIR(6) | A_BOLD);

    old_paddle_x = paddle_x; // actualizam pozitia paletei

    //dc jocul nu e pierdut, desenam mingea
    if (stare_finala != 2) {
        old_ball_x = (int)ball_x;
        old_ball_y = (int)ball_y;
        wattron(win, COLOR_PAIR(7) | A_BOLD);
        mvwaddch(win, old_ball_y, old_ball_x, 'O');
        wattroff(win, COLOR_PAIR(7) | A_BOLD);
    }

    if (!joc_lansat && stare_finala == 0) {
        wattron(win, COLOR_PAIR(3) | A_BOLD);
        mvwprintw(win, WIN_H / 2 + 2, (WIN_W - 44) / 2,
                  "Apasa SPACE pentru a lansa mingea! (Q = Iesire)");
        wattroff(win, COLOR_PAIR(3) | A_BOLD);
    } else if (stare_finala == 1) {
        wattron(win, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(win, WIN_H / 2, (WIN_W - 38) / 2,
                  "FELICITARI! Ai spart toate bricksle!");
        mvwprintw(win, WIN_H / 2 + 2, (WIN_W - 36) / 2,
                  "Scor final: %d | R = Restart | Q = Quit", scor);
        wattroff(win, COLOR_PAIR(4) | A_BOLD);
    } else if (stare_finala == 2) {
        wattron(win, COLOR_PAIR(1) | A_BOLD);
        mvwprintw(win, WIN_H / 2, (WIN_W - 28) / 2,
                  "AI PIERDUT! Mingea a cazut.");
        mvwprintw(win, WIN_H / 2 + 2, (WIN_W - 36) / 2,
                  "Scor final: %d | R = Restart | Q = Quit", scor);
        wattroff(win, COLOR_PAIR(1) | A_BOLD);
    }
    // dupa modificarile facute, actualizam fereastra

    wrefresh(win);
}

void actualizare_stare_joc() {
    int ok = 0;
    if (!joc_lansat || stare_finala != 0) return;

    double next_x = ball_x + ball_dx; //calc unde ar trb sa ajunga mingea la poz urm
    double next_y = ball_y + ball_dy;

    if (next_x < 1.0) { //dc atinge marginea din stg
        next_x = 1.0;   // o mutam cu o pozitie in interior
        ball_dx = -ball_dx; // isi schimba directia
    } else if (next_x >= (double)(WIN_W - 1)) { //dc atinge marginea din dreapta
        next_x = (double)(WIN_W - 2);
        ball_dx = -ball_dx;
    }

    if (next_y < 1.0) { //dc atinge marginea superioara
        next_y = 1.0;
        ball_dy = -ball_dy;
    }

    //coliziunea cu paleta - mingea atinge paleta si se deplaseaza in jos
    if ((int)next_y == PADDLE_Y && ball_dy > 0) {
        if ((int)next_x >= paddle_x && (int)next_x < paddle_x + PADDLE_W) {
            next_y = PADDLE_Y - 1; //o mutam deasupra paletei
            ball_dy = -BALL_SPEED_Y; //o trimitem in sus

            int lovitura = (int)next_x - (paddle_x + PADDLE_W / 2); //unde a lovit mingea paleta in fctie de centrul acesteia
            if (lovitura < -2) ball_dx = -0.16; 
            else if (lovitura < 0) ball_dx = -0.10;
            else if (lovitura == 0) ball_dx = (ball_dx > 0) ? 0.06 : -0.06; // exact pe centru, pastram cond anterioara orizontala
            else if (lovitura <= 2) ball_dx = 0.10;
            else ball_dx = 0.16;
        }
    }

    //daca atinge baza inferioara a ferestrei, pierde
    if ((int)next_y >= WIN_H - 1) {
        stare_finala = 2;
        return;
    }

    int bx = (int)next_x;
    int by = (int)next_y;
    //verificam coliziunea cu caramizile
    for (int i = 0; i < BRICK_ROWS && ok == 0; i++) {
        for (int j = 0; j < BRICK_COLS && ok==0 ; j++) {
            if (bricks[i][j].on) {
                if (by == bricks[i][j].y && //pe acelasi rand cu caramida
                    bx >= bricks[i][j].x && //de la inceputul ei 
                    bx < bricks[i][j].x + BRICK_W) { // sa nu depaseasca 

                    bricks[i][j].on = 0; //dezactivam caramida, o eliminam
                    scor += bricks[i][j].points;
                    bricks_ramase--;

                    if ((int)ball_y != bricks[i][j].y) {
                        ball_dy = -ball_dy; 
                    } else if ((int)ball_x != bricks[i][j].x) {
                        ball_dx = -ball_dx;
                    }

                    if (bricks_ramase == 0) {
                        stare_finala = 1; 
                        ok = 1;
                        ball_x = next_x;
                        ball_y = next_y;
                    } //am castigat daca am nimerit toate caramizile
                      
                     //nu mai verificam restul caramizilor, sarim la eticheta si actualizam pozitiile 
                }
            }
        }
    }
    
    ball_x = next_x;
     ball_y = next_y;
     


}

int main() {
    srand(time(NULL));

    initscr();
    cbreak(); //permite citirea tastelor fara enter
    noecho(); //cand apasam o tasta, nu mai e afisata direct pe ecran
    curs_set(0); //ascunde cursorul terminalului

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);
        init_pair(6, COLOR_WHITE, COLOR_BLACK);
        init_pair(7, COLOR_YELLOW, COLOR_BLACK);
    }

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x); //dim ferestrei

    //pozitionam jocul in mijlocul terminalului
    int start_y = (max_y > WIN_H) ? (max_y - WIN_H) / 2 : 0;
    int start_x = (max_x > WIN_W) ? (max_x - WIN_W) / 2 : 0;

    WINDOW *win = newwin(WIN_H, WIN_W, start_y, start_x);
    keypad(win, TRUE); //permite recunoasterea sagetilor
    nodelay(win, TRUE); //permite citirea tastelor fara blocarea ecranului

    mvprintw(start_y + WIN_H, start_x,
             "Controale: [<-/->] Paleta | [SPACE] Start | [R] Restart | [Q] Quit");
    refresh();

    init_joc();

    int ch; //tasta apasata de user
    while (1) {
        while ((ch = wgetch(win)) != ERR) { //utilizatorul a tastat
            if (ch == 'q' || ch == 'Q') {
                delwin(win);
                endwin();
                return 0;
            }

            if ((ch == 'r' || ch == 'R') && stare_finala != 0) {
                werase(win);
                init_joc();
                continue;
            }

            //deplasam paleta, asigurandu-ne ca nu atinge marginile ferestrei
            if (stare_finala == 0) {
                if (ch == KEY_LEFT) {
                    paddle_x -= PADDLE_SPEED;
                    if (paddle_x < 1) paddle_x = 1;
                    if (!joc_lansat) ball_x = paddle_x + PADDLE_W / 2;
                } else if (ch == KEY_RIGHT) {
                    paddle_x += PADDLE_SPEED;
                    if (paddle_x + PADDLE_W > WIN_W - 1) paddle_x = WIN_W - 1 - PADDLE_W;
                    if (!joc_lansat) ball_x = paddle_x + PADDLE_W / 2;
                } else if ( ch == ' ' && !joc_lansat) {
                    werase(win);
                    joc_lansat = 1;
                }
            }
        }

        actualizare_stare_joc(); //calc noua pozitie a mingii, o deplaseaza si verif toate coliziunile
        deseneaza_fereastra(win); 

        usleep(15000); //pauza de 15 ms
    }
}