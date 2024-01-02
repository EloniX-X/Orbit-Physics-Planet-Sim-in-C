#include <SDL.h>
#include <stdio.h>
#include <unistd.h> //# i think this is for the sleep function
#include <time.h>
#include <stdlib.h>

int pc = 0;

//lets just say

typedef struct {
    int x;
    int y;
    float velx, vely;
    float mass;
    float radius;
    char col;
} Point;

void draw_filled_circle(SDL_Renderer* renderer, Point *p, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int x = p->x;
    int y = p->y;
    int radius = p->radius;
   // SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;  
            int dy = radius - h;  
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

const int BORDER_LEFT = 0;
const int BORDER_RIGHT = 600;  
const int BORDER_TOP = 0;
const int BORDER_BOTTOM = 600; 
int spawningopt = 0;
//Point points[30];

float distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void checkbord(Point *p, int radius) {
    if (p->x - radius < BORDER_LEFT) {
        p->x = BORDER_LEFT + radius;      
        p->velx *= -1;            
    } else if (p->x + radius > BORDER_RIGHT) {
        p->x = BORDER_RIGHT - radius;
        p->velx *= -1;
    }

    if (p->y - radius < BORDER_TOP) {
        p->y = BORDER_TOP + radius;        
        p->vely *= -1;          
    } else if (p->y + radius > BORDER_BOTTOM) {
        p->y = BORDER_BOTTOM - radius;
        p->vely *= -1;
    }
}



void removePoint(int index, Point *points, int *pc) {
    printf("ran");
    if (index < 0 || index >= *pc) {
        
        return;
    }

    for (int i = index; i < (*pc) - 1; i++) {
        points[i] = points[i];
    }
    (*pc)--;
    printf("ok its out \n");
}

int circles_overlap(Point p1, Point p2, int radius1, int radius2) {
 
    float dist = distance(p1.x, p1.y, p2.x, p2.y);
    
 
    if (dist < (radius1 + radius2)) {
        return 1;  
    } else {
        return 0;  
    }
}

void gravitate(int G, Point *p1, Point *p2, int i, int j, Point *points, SDL_Renderer* renderer, int redeact, int greenreact) {
    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float distanceSquared = dx*dx + dy*dy;
    
    if (distanceSquared <= 1) { 
        p1->mass += p2->mass;  
        p1->x = (p1->x * p1->mass + p2->x * p2->mass) / (p1->mass + p2->mass);  
        p1->y = (p1->y * p1->mass + p2->y * p2->mass) / (p1->mass + p2->mass);
        p1->velx = (p1->velx * p1->mass + p2->velx * p2->mass) / (p1->mass + p2->mass); 
        p1->vely = (p1->vely * p1->mass + p2->vely * p2->mass) / (p1->mass + p2->mass);

        removePoint(j, points, &pc);
        int radius = +15;
        float volume1 = (4.0/3) * M_PI * pow(p1->radius, 3);
        float volume2 = (4.0/3) * M_PI * pow(p2->radius, 3);
        float totalVolume = volume1 + volume2;
        p1->radius = cbrt((3.0/4) * totalVolume / M_PI);
        SDL_Color color = {255, 225, 225, 255};
        draw_filled_circle(renderer, p1, color);

        distanceSquared = 1;
    }
    int max_cap = 4;
    float distance = sqrt(distanceSquared);
    float force = (G * p1->mass * p2->mass) / distanceSquared + 1;
    float forceX = (dx / distance) * force;
    float forceY = (dy / distance) * force;

    p1->velx += forceX / p1->mass;
    p1->vely += forceY / p1->mass;
    p2->velx -= forceX / p2->mass;
    p2->vely -= forceY / p2->mass;


}

const int FPS = 60;
const int frameDelay = 1000 / FPS;

Uint32 frameStart;
int frameTime;
int ispaused = 0;   
int main() {
    Point points[30];
    SDL_Init(SDL_INIT_VIDEO); 
    SDL_Window* window = SDL_CreateWindow("Physics Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    
    for (;;) {
        SDL_Event e;
       
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            break;
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {  
               
                
                int clickX = e.button.x;
                int clickY = e.button.y;
                printf("Left click at X: %d, Y: %d\n", clickX, clickY);
                points[pc++] = (Point){clickX, clickY, 0, 0, 1, 2};
                
        
            }
            } else if (e.type == SDL_KEYDOWN) {
                if  (e.key.keysym.sym == SDLK_SPACE) {
                    if (ispaused == 1) {
                        ispaused = 0;
                    } else {
                        ispaused = 1;
                    }
                } 
            } 
        }
    



    if (e.type == SDL_QUIT) break;

        if (SDL_PollEvent(&e) && e.type == SDL_QUIT) break;
        frameStart = SDL_GetTicks();
       
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 225, 225, 225, 225);

    
            //int startX = x - radius;
           // int startY = y - radius;

   
        for (int i = 0; i < pc; i++) {
            SDL_Color color = {255, 255, 255, 255}; 
           // draw_filled_circle(renderer, points[i].x, points[i].y, 49,(SDL_Color){255, 0, 0, 255});
            draw_filled_circle(renderer, &points[i], color);
        }
        int G = -2;
        if (ispaused == 0) {
            for (int i = 0; i < pc; i++) {
            
                checkbord(&points[i], 3);
            // points[i].velx = -3;
                points[i].x += points[i].velx;
                points[i].y += points[i].vely;
                for (int j = i + 1; j < pc; j++) {
                if (circles_overlap(points[i], points[j], 30, 30)) {
                 //   printf("Circles %d and %d overlap.\n", i, j);
                    gravitate(G, &points[i], &points[j], i, j, points, renderer, 2, 1);
                }
                }

            }
        } else {
            printf("hi");
        }
        


//move left slowly
//big circle
            
    //    draw_filled_circle(renderer, point1.x, point1.y, radius);
        


        SDL_RenderPresent(renderer);
        frameTime = SDL_GetTicks() - frameStart;

        if(frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }

    
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


//gcc -o main main.c `sdl2-config --cflags --libs`
// then do ./main