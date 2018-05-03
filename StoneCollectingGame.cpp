#include <stdio.h>
#include <allegro5\allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include "Obj.h"

const int WIDTH = 750;
const int HEIGHT = 600;
const int NUM_STONE = 10;
enum KEYS {UP,DOWN,LEFT,RIGHT,SPACE};
bool keys[5] = {false, false, false, false,false};

void initPlayer(PLAYER &player);
void DrawPlayer(PLAYER &player);
void MovePlayerUp(PLAYER &player);
void MovePlayerDown(PLAYER &player);
void MovePlayerLeft(PLAYER &player);
void MovePlayerRight(PLAYER &player);
void BoostStone(STONE stones[],PLAYER &player, int size);

void initStone(STONE stones[], int size);
void DrawStone(STONE stones[], int size);
void StartStone(STONE stones[], int size);
void UpdateStone(STONE stones[], int size);
void CollideStone(STONE stones[], int cSize, PLAYER &player);

void initBackground(Background &back, float x, float y, float velx, float vely, int width, int height, int dirX, int dirY, ALLEGRO_BITMAP *image);
void UpdateBackground(Background &back);
void DrawBackground(Background &back);

int main()
{
    bool done = false;
    bool redraw = true;
    const int FPS = 60;
    bool isgameover = false;
    int sum = 10, life = 9,x = 3;

    Background BG;

    PLAYER player;
    STONE stones[NUM_STONE];

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *font18 = NULL;
    ALLEGRO_BITMAP *bgImage = NULL;

    if(!al_init())
    {
        printf("Allegro don't Init\n");
        return -1;
    }
    display = al_create_display(WIDTH, HEIGHT);
    if(!display)
    {
        printf("Display Don't Create\n");
        return -1;
    }
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0/FPS);

    bgImage = al_load_bitmap("images/starBG.png");

    srand(time(NULL));
    initPlayer(player);
    initStone(stones, NUM_STONE);

    initBackground(BG, 0, 0, 5, 0, WIDTH, HEIGHT, -1, 1, bgImage);

    font18 = al_load_font("fonts/arial.ttf", 18, 0);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_start_timer(timer);
    while(!done)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;
            if(keys[UP])
                MovePlayerUp(player);
            if(keys[DOWN])
                MovePlayerDown(player);
            if(keys[LEFT])
                MovePlayerLeft(player);
            if(keys[RIGHT])
                MovePlayerRight(player);
            if(keys[SPACE])
            {
                if(player.Boost>0)
                {
                   BoostStone(stones,player, NUM_STONE);
                   player.Boost--;
                   keys[SPACE] = false;
                }
                //x--;
            }
            if(!isgameover)
            {
                UpdateBackground(BG);

                StartStone(stones, NUM_STONE);
                UpdateStone(stones, NUM_STONE);
                CollideStone(stones, NUM_STONE,player);

                if(player.lives <=0)
                {
                    isgameover = true;
                }
            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            case ALLEGRO_KEY_UP:
                keys[UP] = true;
                break;
            case ALLEGRO_KEY_DOWN:
                keys[DOWN] = true;
                break;
            case ALLEGRO_KEY_LEFT:
                keys[LEFT]= true;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT] = true;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = true;
                break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            case ALLEGRO_KEY_UP:
                keys[UP] = false;
                break;
            case ALLEGRO_KEY_DOWN:
                keys[DOWN] = false;
                break;
            case ALLEGRO_KEY_LEFT:
                keys[LEFT]= false;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT]= false;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = false;
                break;
            }
        }
        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            if(!isgameover)
            {
                DrawBackground(BG);

                DrawPlayer(player);
                DrawStone(stones, NUM_STONE);
                if(player.score>=sum)
                {
                    player.level++;
                    player.Boost = ++x;
                    sum*=2;
                    player.lives = life;
                    life--;
                    if(life<=3) life = 3;
                }
                al_draw_textf(font18, al_map_rgb(25, 10, 245), 5, 5, 0, "Life : %i. Score : %i. Level : %i Boost : %i", player.lives, player.score,player.level, player.Boost);
            }
            else
            {
                al_draw_textf(font18, al_map_rgb(255, 0, 0), WIDTH/2, HEIGHT/2-40, ALLEGRO_ALIGN_CENTER, "Game Over");
                al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH/2, HEIGHT/2-20, ALLEGRO_ALIGN_CENTER, "You Complete : %i \level", player.level-1);
                al_draw_textf(font18, al_map_rgb(0, 255, 0), WIDTH/2, HEIGHT/2, ALLEGRO_ALIGN_CENTER, "Final Score : %i", player.score);
            }
            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        }
    }
    al_destroy_bitmap(bgImage);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font18);
    al_destroy_display(display);

    return 0;
}
void initPlayer(PLAYER &player)
{
    player.x = WIDTH/2;
    player.y = HEIGHT - 20;
    player.lives = 10;
    player.speed = 7;
    player.boundx = 43;
    player.boundy = 30;
    player.score = 0;
    player.level = 1;
    player.Boost = 3;
}
void DrawPlayer(PLAYER &player)
{

    al_draw_filled_circle(player.x-30,player.y-100,20,al_map_rgb(0,255,0));
    al_draw_filled_rectangle(player.x-70, player.y-80, player.x + 10 , player.y + 10, al_map_rgb(255, 0, 0));
    al_draw_filled_rectangle(player.x+10, player.y - 70, player.x + 20 , player.y - 17, al_map_rgb(0, 255, 0));
    al_draw_filled_rectangle(player.x-80, player.y - 70, player.x - 70 , player.y - 17, al_map_rgb(0, 255, 0));
}
void MovePlayerUp(PLAYER &player)
{
    player.y  = player.y - player.speed;
    if(player.y <HEIGHT/2+100)
        player.y = HEIGHT/2+100;
}
void MovePlayerDown(PLAYER &player)
{
    player.y = player.y + player.speed;
    if(player.y >HEIGHT)
        player.y = HEIGHT;
}
void MovePlayerLeft(PLAYER &player)
{
    player.x = player.x -  player.speed;
    if(player.x < 0)
        player.x = 0;
}
void MovePlayerRight(PLAYER &player)
{
    player.x = player.x + player.speed;
    if(player.x > WIDTH)
        player.x = WIDTH;
}
void BoostStone(STONE stones[],PLAYER &player, int size)
{
    for(int i = 0; i<size; i++)
    {
        if(stones[i].live)
        {
            stones[i].live = false;
            player.score++;
        }
    }
    //player.Boost--;
}
void initStone(STONE stones[], int size)
{
    for(int i = 0; i<size; i++)
    {
        stones[i].live = false;
        stones[i].speed = 5;
        stones[i].boundx = 18;
        stones[i].boundy = 18;
    }
}
void DrawStone(STONE stones[], int size)
{
    for(int i = 0; i<size; i++)
    {
        if(stones[i].live)
        {
            al_draw_filled_circle(stones[i].x, stones[i].y, 20, al_map_rgb(246,255,003));
        }
    }
}
void StartStone(STONE stones[], int size)
{
    for(int i = 0; i<size; i++)
    {
        if(!stones[i].live)
        {
            if(rand() % 500 == 0)
            {
                stones[i].live = true;
                stones[i].y = 0;
                if((30+rand()%(WIDTH-60))>=5 && (30+rand()%(WIDTH-60))<=WIDTH-5)
                {
                    stones[i].x = 30 + rand() % (HEIGHT - 60);
                }
                break;
            }
        }
    }
}
void UpdateStone(STONE stones[], int size)
{
    for(int i = 0; i<size; i++)
    {
        if(stones[i].live)
        {
            stones[i].y += stones[i].speed;
        }
    }
}
void CollideStone(STONE stones[], int cSize, PLAYER &player)
{
    for(int i = 0; i<cSize; i++)
    {
        if(stones[i].live)
        {
            if(stones[i].x - stones[i].boundx < player.x + player.boundx &&
                    stones[i].x + stones[i].boundx > player.x - player.boundx &&
                    stones[i].y - stones[i].boundy < player.y + player.boundy &&
                    stones[i].y + stones[i].boundy > player.y - player.boundy)
            {
                player.score++;
                stones[i].live = false;
            }
            else  if(stones[i].y > HEIGHT )
            {
                stones[i].live = false;
                player.lives--;
            }
        }
    }
}

void initBackground(Background &back, float x, float y, float velx, float vely, int width, int height, int dirX, int dirY, ALLEGRO_BITMAP *image)
{
    back.x = x;
    back.y = y;
    back.velX = velx;
    back.velY = vely;
    back.width = width;
    back.height = height;
    back.dirX = dirX;
    back.dirY = dirY;
    back.image = image;
}
void DrawBackground(Background &back)
{
    al_draw_bitmap(back.image, back.x, back.y, 0);

    if(back.x + back.width < WIDTH)
        al_draw_bitmap(back.image, back.x + back.width, back.y, 0);
}
void UpdateBackground(Background &back)
{
    back.x += back.velX * back.dirX;
    if(back.x + back.width <= 0)
        back.x = 0;
}
