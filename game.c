#include <stdio.h>
#include <stdbool.h>
#include <math.h>


#include <SDL.h>
#include <stdio.h>

#include "myStructs.h"

#include "server.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


float PlayerSpeed = 2;
float PlayerAcceleration = 0.6f;
float FrameTime = 10.0f;
float drag = 3.0f;

Ball gameBall = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, M_PI, M_PI, 10};

int PlayerW = 20;
int PlayerH = 50;


//client side server variables
int status, valread, client_fd;
struct sockaddr_in serv_addr;
char* hello = "Hello from client";
char buffer[1024] = { 0 };

// x and y
PlayerCoords Player = {50, 0};

// dx dy
Movement CurrentMove = {0, 0};

void moveBall() {
    gameBall.x += gameBall.dx;
    gameBall.y += gameBall.dy;

    // Wall collisions
    if (gameBall.x + gameBall.r >= SCREEN_WIDTH) {
        gameBall.x = SCREEN_WIDTH - gameBall.r - 1;
        gameBall.dx *= -1;
    }
    if (gameBall.x - gameBall.r < 0) {
        gameBall.x = gameBall.r + 1;
        gameBall.dx *= -1;
    }
    if (gameBall.y - gameBall.r < 0) {
        gameBall.y = gameBall.r + 1;
        gameBall.dy *= -1;
    }
    if (gameBall.y + gameBall.r > SCREEN_HEIGHT) {
        gameBall.y = SCREEN_HEIGHT - gameBall.r - 1;
        gameBall.dy *= -1;
    }

    // Player collision
    //sito rakstija chatgpt jo man ir reali galaks rakstit bulba + taisnsturis logiku :D
    float closestX = (gameBall.x < Player.x) ? Player.x : 
                     (gameBall.x > Player.x + PlayerW) ? Player.x + PlayerW : gameBall.x;
    float closestY = (gameBall.y < Player.y) ? Player.y : 
                     (gameBall.y > Player.y + PlayerH) ? Player.y + PlayerH : gameBall.y;

    // Distance between the circle's center and this closest point
    float distX = gameBall.x - closestX;
    float distY = gameBall.y - closestY;
    float distance = sqrt((distX * distX) + (distY * distY));

    // Check if the distance is less than or equal to the radius
    if (distance <= gameBall.r) {
        // Reflect the ball's direction
        // Horizontal reflection if collision is on left/right sides of the paddle
        if (closestX == Player.x || closestX == Player.x + PlayerW) {
            gameBall.dx *= -1;
            // Move the ball away from the player to prevent sticking
            if (gameBall.x < Player.x) {
                gameBall.x = Player.x - gameBall.r;  // Move the ball to the left of the player
            } else {
                gameBall.x = Player.x + PlayerW + gameBall.r;  // Move the ball to the right of the player
            }
        }
        // Vertical reflection if collision is on top/bottom sides of the paddle
        if (closestY == Player.y || closestY == Player.y + PlayerH) {
            gameBall.dy *= -1;
            // Move the ball away from the player to prevent sticking
            if (gameBall.y < Player.y) {
                gameBall.y = Player.y - gameBall.r;  // Move the ball above the player
            } else {
                gameBall.y = Player.y + PlayerH + gameBall.r;  // Move the ball below the player
            }
        }
    }
}


//center x and y btw
void drawBall (SDL_Renderer* renderer, float cX, float cY, float radius){
	SDL_SetRenderDrawColor(renderer, 255, 255,255,255);
	for (float w =0; w < radius *2; w+= 1){
		for(float h =0; h < radius * 2; h+= 1){
			float dx = radius - w;
			float dy = radius - h;
			if((dx*dx + dy*dy) <= (radius * radius)){
				SDL_RenderDrawPoint(renderer, cX+dx, cY+dy);
			}
		}
	}

}

int sendInfo(char* message) {
    if (client_fd < 0) { // Reconnect if the socket is closed
        if (clientInit() < 0) {
            return -1;
        }
    }

    ssize_t bytes_sent = send(client_fd, message, strlen(message), 0);
    if (bytes_sent < 0) {
        perror("send failed kkapec \n");
        return -1;
    }

    printf("Hello message sent: %s\n", message);
    valread = read(client_fd, buffer, 1024 - 1);
    if (valread < 0) {
        perror("read failed?! \n");
        return -1;
    }
    if (valread == 0) {
        perror("server closed?");
        close(client_fd);
        client_fd = -1; // Mark the socket as closed
        return -1;
    }

    buffer[valread] = '\0';
    printf("Serveris atbild: %s\n", buffer);
    return 0;
}


void updatePlayerCurrentMove(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		printf("00000\n");
		if (sendInfo("test test test yoyoyoy?") != 0) {
			printf("Error sending message\n");
			close(client_fd);
			return;
		}
		printf("444444\n");

		if (event.key.keysym.sym == SDLK_a) {
			//CurrentMove.x += -PlayerAcceleration;
		} else if (event.key.keysym.sym == SDLK_w) {
			CurrentMove.y = -PlayerSpeed;
		} else if (event.key.keysym.sym == SDLK_d) {
			//CurrentMove.x += PlayerAcceleration;
		} else if (event.key.keysym.sym == SDLK_s) {
			CurrentMove.y = PlayerSpeed;
		}
	}
	
	if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_a) {
                        //CurrentMove.x += -PlayerAcceleration;
                } else if (event.key.keysym.sym == SDLK_w) {
                        CurrentMove.y = 0;
                } else if (event.key.keysym.sym == SDLK_d) {
                        //CurrentMove.x += PlayerAcceleration;
                } else if (event.key.keysym.sym == SDLK_s) {
                        CurrentMove.y = 0;
                }
        }


}


int clientInit(){
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("\n Socket creation error \n");
                return -1;
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
                printf("\nInvalid address/ Address not supported \n");
                return -1;
        }

        if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
                printf("\nConnection Failed \n");
                return -1;
        }

	return 0;
}

int main() {
	printf("Normunda Spelite \n \n \n");
	//startServer();
	
	clientInit();
	
	SDL_Window* window = NULL;
	// SDL_Surface* screenSurface = NULL;
	SDL_Renderer* renderer = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL dirsaa neiet ajaj %s, ", SDL_GetError());
	} else {
		// SDL Initojaas
		window = SDL_CreateWindow("Normunda Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			printf("window nesanaca uztaisit %s", SDL_GetError());
			SDL_DestroyWindow(window);
			SDL_Quit();
			return 1;
		} else {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL) {
				printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
				SDL_DestroyWindow(window);
				SDL_Quit();
				return 1;
			}

			bool quit = false;
			SDL_Event event;

			while (!quit) {
				//printf("paga sitais printejas vne? \n");
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						printf("seit iegaaja? \n");
						quit = true;
					} else {
						updatePlayerCurrentMove(event);
					}
				}

				//applyDrag();

				// pakustina player object
				Player.x += CurrentMove.x;
				Player.y += CurrentMove.y;
				
				if(Player.y <= 0){
					Player.y = 0;
					CurrentMove.y = 0;
				}

				if(Player.y + 50 >= SCREEN_HEIGHT){
					CurrentMove.y = 0;
					Player.y = SCREEN_HEIGHT - 50;
				}
				

				// zimejam playeri

				// saakas rectangle
				SDL_Rect r = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
				SDL_SetRenderDrawColor(renderer, 70,70,70, 255);
				SDL_RenderFillRect(renderer, &r);


				drawBall(renderer, gameBall.x, gameBall.y, gameBall.r);
				moveBall();


				//es taisu pingpong x kustibu jaiznem pilniba bet lai paliek es lepns jutos
				//r.x = Player.x;
				r.x = Player.x;
				r.y = Player.y;
				r.w = PlayerW;
				r.h = PlayerH;
				SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
				SDL_RenderFillRect(renderer, &r);

				// update
				SDL_RenderPresent(renderer);

				SDL_Delay(FrameTime);
			}

			//serverim;
                        printf("sis aizver ciet visu un pizda \n");
                        close(client_fd);


			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
	}

	return 0;
}

