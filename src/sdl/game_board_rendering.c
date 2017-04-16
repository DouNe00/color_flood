#include "game_board_rendering.h"

LButton** boardInitButtons(game *g, SDL_Window **gWindow, SDL_Renderer **gRenderer) {
	int i;
	LButton** colorButtons = (LButton **)calloc(g->cNb, sizeof(LButton *));
	buttonSide = (SCREEN_WIDTH - SIDES_MARGINS * 2 - BUTTON_MARGIN * (g->cNb -1)) / g->cNb;

	for(i=0; i<g->cNb; i++) {
		colorButtons[i] = sdlCreateButton(gWindow,
										  gRenderer,
										  NULL,
										  NULL,
										  g->cTab[i].R,
										  g->cTab[i].G,
										  g->cTab[i].B,
										  buttonSide,
										  buttonSide,
										  SIDES_MARGINS + i * (buttonSide + BUTTON_MARGIN),
										  SCREEN_HEIGHT - SIDES_MARGINS - buttonSide,
										  &colorButtonAction);
	}

	return colorButtons;
}

void boardHandleEvents(LButton **boardButtons, int buttonsCount, SDL_Event *e, int *playerColor) {
	int i;
	for(i=0; i<buttonsCount; i++) {
		/* maybe make a function in button_wrapper to check if inside */
		if(e->type == SDL_MOUSEBUTTONUP) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			LButton *b = boardButtons[i];

			bool inside = true;

			if(x < b->mPosition.x) {
				inside = false;
			}
			else if(x > b->mPosition.x + LTexture_GetWidth(b->mBackground)) {
				inside = false;
			}
			else if(y < b->mPosition.y) {
				inside = false;
			}
			else if(y > b->mPosition.y + LTexture_GetHeight(b->mBackground)) {
				inside = false;
			}
			if(inside) {
				(*playerColor) = boardButtons[i]->mPosition.x;
			}
		}

		LButton_HandleEvent(boardButtons[i], e, playerColor);
	}
}

void boardRenderButtons(LButton **boardButtons, int buttonsCount) {
	int i;
	for(i=0; i<buttonsCount; i++) {
		LButton_Render(boardButtons[i]);
	}
}

void boardRenderTile(SDL_Renderer **gRenderer, int x, int y, size_t side, RGB color) {
	SDL_SetRenderDrawColor(*gRenderer, color.R, color.G, color.B, 0xFF);		
	SDL_Rect tile = {x, y, side, side};
	SDL_RenderFillRect(*gRenderer, &tile);
}

void boardRenderBoard(game *g, SDL_Renderer **gRenderer) {
	int availableVertical = SCREEN_HEIGHT - 3 * SIDES_MARGINS - buttonSide;
	int availableHorizontal = SCREEN_WIDTH - 2 * SIDES_MARGINS;
	int tileSide = ((availableVertical < availableHorizontal) ? availableVertical : availableHorizontal) / g->size;
	/* keep it centered despite rounding */
	int offset = (availableHorizontal - tileSide * g->size) / 2;
	int i,j,x,y;
	for(i=0; i<g->size; i++) {
		for(j=0; j<g->size; j++) {
			x = SIDES_MARGINS + i * tileSide + offset;
			y = SIDES_MARGINS + j * tileSide;
			boardRenderTile(gRenderer, x, y, tileSide, gridGetColor(g->grid, i, j));
		}
	}
}

GameState boardRoutine(game *g, SDL_Window **gWindow, SDL_Renderer **gRenderer, SDL_Event *e) {
	int playerColor = -1;
	GameState gs = GAMESTATE_PLAYING;
	LButton **boardButtons = boardInitButtons(g, gWindow, gRenderer);
	
	while(gs == GAMESTATE_PLAYING) {
		SDL_SetRenderDrawColor(*gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(*gRenderer);

		while(SDL_PollEvent(e) != 0) {
			if(e->type == SDL_QUIT) {
				gs = GAMESTATE_QUIT;
			}
			else {
				boardHandleEvents(boardButtons, g->cNb, e, &playerColor);
			}
		}

		gamePlayTurnSDL(g, playerColor);

		boardRenderButtons(boardButtons, g->cNb);
		boardRenderBoard(g, gRenderer);

		SDL_RenderPresent(*gRenderer);

		if(gameOver(g)) {
			/* TODO : GAMESTATE_GAMEOVER */
			gs = GAMESTATE_MENU;
		}
	}
	int i;
	for(i=0; i<g->cNb; i++) {
		LButton_Delete(boardButtons[i]);
		boardButtons[i] = NULL;
	}

	return gs;
}

void colorButtonAction(int *playerColor) {
	/* x coodinate -> color index */
	int newColor = ((*playerColor) - SIDES_MARGINS) / (buttonSide + BUTTON_MARGIN);
	(*playerColor) = newColor;
}
