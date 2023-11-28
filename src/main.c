#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "video.h"
#include "timer.h"
#include "keyboard.h"

#include <graph.h>

#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define SPRITE_SIZE ((TILE_WIDTH / 4) * TILE_HEIGHT)

#define MAX_BOMBS 4

char TmpSprite[SPRITE_SIZE * 2];

int* Music;
long IMusic;
long LenMusic;

int indexes[] = {0, 1, 2, 1};

char map[] = {
	"wwwwwwwwwwwwwwwwwww"
	"w        b        w"
	"w   w   w   w   w w"
	"w        b        w"
	"w w   w   w   w   w"
	"wb b b b b b b b bw"
	"w   w   w   w   w w"
	"w        b        w"
	"w w   w   w   w   w"
	"w        b        w"
	"wwwwwwwwwwwwwwwwwww"
};

struct {
	int row;
	int col;
	int time_to_explode;
	int active;
}Bombs[MAX_BOMBS];

struct {
	int row;
	int col;
	int active;
	int i_anim;
	int num_anim;
	char* sprite;
}Explosions[MAX_BOMBS * 9];

typedef struct {
	int x;
	int y;
	int w;
	int h;
}rect;

void rect_union(rect* out, const rect* a, const rect* b) {
	out->x = min(a->x, b->x);
	out->y = min(a->y, b->y);
	out->w = max(a->x + a->w, b->x + b->w) - out->x;
	out->h = max(a->y + a->h, b->y + b->h) - out->y;
}

long filesize(FILE* fp) {
	long save_pos;
	long size_of_file;

	save_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	size_of_file = ftell(fp);
	fseek(fp, save_pos, SEEK_SET);

	return size_of_file;
}

static void free_resources(void);

static long load_resource(const char* name, char** resource) {
	FILE* fp;
	long len_file;
	char* tmp;

	fp = fopen(name, "rb");
	len_file = filesize(fp);
	tmp = (char*)malloc(len_file);
	if (tmp == NULL) {
		printf("Not enough memory for %s\n", name);
		free_resources();
		exit(1);
	}
	fread(tmp, len_file, 1, fp);
	fclose(fp);

	*resource = tmp;

	return len_file;
}

static void free_resource(char** resource) {
	char* tmp = *resource;

	if (tmp != NULL) {
		free(tmp);
		*resource = NULL;
	}
}

char* walk_up = NULL;
char* walk_down = NULL;
char* walk_left = NULL;
char* walk_right = NULL;
char* wall = NULL;
char* brick = NULL;
char* bomb = NULL;
char* euu = NULL;
char* emu = NULL;
char* ell = NULL;
char* eml = NULL;
char* emm = NULL;
char* emr = NULL;
char* err = NULL;
char* emd = NULL;
char* edd = NULL;

int main(int argc, char* argv[]) {
	int old_mode;
	char* pg;
	int x = 0;
	int y = 0;
	int row = 0;
	int col = 0;
	int i = 0;
	int i_map = 0;
	int i_explosion = 0;
	int i_anim_pg = 0;
	int i_anim_bomb = 0;
	int next_frame;
	int moving;
	int continua_esplosione;
	int prev_space = 0;
	rect player_rect = {0, 0, TILE_WIDTH, TILE_HEIGHT};
	rect old_player_rect = {0, 0, TILE_WIDTH, TILE_HEIGHT};
	rect redraw_rect = {0, 0, TILE_WIDTH, TILE_HEIGHT};

	LenMusic = load_resource("game.snd", (char**)&Music) / sizeof(int);

	load_resource("wk_up.sht", &walk_up);
	load_resource("wk_dn.sht", &walk_down);
	load_resource("wk_lt.sht", &walk_left);
	load_resource("wk_rt.sht", &walk_right);
	load_resource("wall.sht", &wall);
	load_resource("brick.sht", &brick);
	load_resource("bomb.sht", &bomb);
	load_resource("euu.sht", &euu);
	load_resource("emu.sht", &emu);
	load_resource("ell.sht", &ell);
	load_resource("eml.sht", &eml);
	load_resource("emm.sht", &emm);
	load_resource("emr.sht", &emr);
	load_resource("err.sht", &err);
	load_resource("emd.sht", &emd);
	load_resource("edd.sht", &edd);

	old_mode = set_video_mode(CGA_MODE_04);
	set_palette(PALETTE_COOL, HIGH_INTENSITY | DARK_GREEN);

	init_timer();
	init_beeper();
	init_keyboard();

	for (y = 0; y < 11; y++) {
		for (x = 0; x < 19; x++) {
			i_map = (y * 19) + x;
			if (map[i_map] == 'w') {
				draw_image(x * TILE_WIDTH, y * TILE_HEIGHT, wall, TILE_WIDTH, TILE_HEIGHT);
			} else if (map[i_map] == 'b') {
				draw_image(x * TILE_WIDTH, y * TILE_HEIGHT, brick, TILE_WIDTH, TILE_HEIGHT);
			}
		}
	}
	update_screen(0, 0, 320, 200);

	//_settextposition(10, 10);
	//cprintf("12345678901234567890");

	x = TILE_WIDTH;
	y = TILE_HEIGHT;

	pg = walk_down;

	read_image(x, y, TmpSprite, TILE_WIDTH, TILE_HEIGHT);
	next_frame = 0;
	while (!(KeyTracker[KEY_ESC] || KeyTracker[KEY_Q])) {
		wait_for_vblank();

		//clear_area(x, y, TILE_WIDTH, TILE_HEIGHT);
		draw_image(x, y, TmpSprite, TILE_WIDTH, TILE_HEIGHT);

		old_player_rect.x = x;
		old_player_rect.y = y;

		moving = 0;
		if (KeyTracker[KEY_LEFT] || KeyTracker[KEY_KP4]) {
			if	(
					(x > 0)
				&&	(map[((y / TILE_HEIGHT) * 19) + ((x - 1) / TILE_WIDTH)] == ' ')
				&&	(map[(((y + TILE_HEIGHT - 1) / TILE_HEIGHT) * 19) + ((x - 1) / TILE_WIDTH)] == ' ')
				) {
				pg = walk_left;
				moving = 1;
				x--;
			}
		}

		if (KeyTracker[KEY_RIGHT] || KeyTracker[KEY_KP6]) {
			if	(
					((x + 1) < (320 - TILE_WIDTH))
				&&	(map[((y / TILE_HEIGHT) * 19) + ((x + TILE_WIDTH) / TILE_WIDTH)] == ' ')
				&&	(map[(((y + TILE_HEIGHT - 1) / TILE_HEIGHT) * 19) + ((x + TILE_WIDTH) / TILE_WIDTH)] == ' ')
				) {
				pg = walk_right;
				moving = 1;
				x++;
			}
		}

		if (KeyTracker[KEY_UP] || KeyTracker[KEY_KP8]) {
			if	(
					(y > 0)
				&&	(map[(((y - 1) / TILE_HEIGHT) * 19) + (x / TILE_WIDTH)] == ' ')
				&&	(map[(((y - 1) / TILE_HEIGHT) * 19) + ((x + TILE_WIDTH - 1) / TILE_WIDTH)] == ' ')
				) {
				pg = walk_up;
				moving = 1;
				y--;
			}
		}

		if (KeyTracker[KEY_DOWN] || KeyTracker[KEY_KP2]) {
			if (
					((y + 1) < (200 - TILE_HEIGHT))
				&&	(map[(((y + TILE_HEIGHT) / TILE_HEIGHT) * 19) + (x / TILE_WIDTH)] == ' ')
				&&	(map[(((y + TILE_HEIGHT) / TILE_HEIGHT) * 19) + ((x + TILE_WIDTH - 1) / TILE_WIDTH)] == ' ')
				) {
				pg = walk_down;
				moving = 1;
				y++;
			}
		}

		if (KeyTracker[KEY_SPACE] && !prev_space) {
			row = (y + (TILE_HEIGHT / 2)) / TILE_HEIGHT;
			col = (x + (TILE_WIDTH / 2)) / TILE_WIDTH;
			if ((!Bombs[0].active) && (map[(row * 19) + col] == ' ')) {
				map[(row * 19) + col] = 'm';
				Bombs[0].row = row;
				Bombs[0].col = col;
				Bombs[0].time_to_explode = 3 * 60;
				Bombs[0].active = 1;
			}
		}

		prev_space = KeyTracker[KEY_SPACE];

		next_frame++;
		if (next_frame > 10) {
			next_frame = 0;
			i_anim_pg++;
			i_anim_pg %= 4;

			i_anim_bomb++;
			i_anim_bomb %= 4;

			for (i_explosion = 0; i_explosion < (MAX_BOMBS * 9); i_explosion++) {
				if (Explosions[i_explosion].active) {
					Explosions[i_explosion].i_anim++;
					if (Explosions[i_explosion].i_anim >= Explosions[i_explosion].num_anim) {
						Explosions[i_explosion].active = 0;
						row = Explosions[i_explosion].row;
						col = Explosions[i_explosion].col;
						map[(row * 19) + col] = ' ';
						clear_area(col * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
						update_screen(col * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					} else {
						row = Explosions[i_explosion].row;
						col = Explosions[i_explosion].col;
						draw_image(col * TILE_WIDTH, row * TILE_HEIGHT, &Explosions[i_explosion].sprite[Explosions[i_explosion].i_anim * SPRITE_SIZE], TILE_WIDTH, TILE_HEIGHT);
						update_screen(col * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}
				}
			}
		}

		if (!moving) {
			i_anim_pg = 1;
		}

		player_rect.x = x;
		player_rect.y = y;

		if (Bombs[0].active) {
			row = Bombs[0].row;
			col = Bombs[0].col;
			Bombs[0].time_to_explode--;
			if (Bombs[0].time_to_explode <= 0) {
				Bombs[0].active = 0;
				map[(row * 19) + col] = ' ';
				Explosions[0].row = row;
				Explosions[0].col = col;
				Explosions[0].sprite = emm;
				Explosions[0].active = 1;
				Explosions[0].i_anim = 0;
				Explosions[0].num_anim = 4;
				draw_image(col * TILE_WIDTH, row * TILE_HEIGHT, &Explosions[0].sprite[0], TILE_WIDTH, TILE_HEIGHT);
				update_screen(col * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
				continua_esplosione = 1;
				if (continua_esplosione && ((col - 1) >= 0)) {
					i_map = (row * 19) + (col - 1);
					if ((map[i_map] == ' ') || (map[i_map] == 'b')) {
						Explosions[1].row = row;
						Explosions[1].col = col - 1;
						Explosions[1].active = 1;
						if (map[i_map] == 'b') {
							Explosions[1].sprite = brick;
							Explosions[1].i_anim = 1;
							Explosions[1].num_anim = 7;
						} else {
							Explosions[1].sprite = eml;
							Explosions[1].i_anim = 0;
							Explosions[1].num_anim = 4;
						}
						draw_image((col - 1) * TILE_WIDTH, row * TILE_HEIGHT, &Explosions[1].sprite[0], TILE_WIDTH, TILE_HEIGHT);
						update_screen((col - 1) * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}

					if (map[i_map] != ' ') {
						continua_esplosione = 0;
					}
				}
				if (continua_esplosione && ((col - 2) >= 0)) {
					i_map = (row * 19) + (col - 2);
					if ((map[i_map] == ' ') || (map[i_map] == 'b')) {
						Explosions[2].row = row;
						Explosions[2].col = col - 2;
						Explosions[2].active = 1;
						if (map[i_map] == 'b') {
							Explosions[2].sprite = brick;
							Explosions[2].i_anim = 1;
							Explosions[2].num_anim = 7;
						} else {
							Explosions[2].sprite = ell;
							Explosions[2].i_anim = 0;
							Explosions[2].num_anim = 4;
						}
						draw_image((col - 2) * TILE_WIDTH, row * TILE_HEIGHT, &Explosions[2].sprite[0], TILE_WIDTH, TILE_HEIGHT);
						update_screen((col - 2) * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}

					if (map[i_map] != ' ') {
						continua_esplosione = 0;
					}
				}
				continua_esplosione = 1;
				if (continua_esplosione && ((col + 1) < 19)) {
					i_map = (row * 19) + (col + 1);
					if ((map[i_map] == ' ') || (map[i_map] == 'b')) {
						Explosions[3].row = row;
						Explosions[3].col = col + 1;
						Explosions[3].active = 1;
						if (map[i_map] == 'b') {
							Explosions[3].sprite = brick;
							Explosions[3].i_anim = 1;
							Explosions[3].num_anim = 7;
						} else {
							Explosions[3].sprite = emr;
							Explosions[3].i_anim = 0;
							Explosions[3].num_anim = 4;
						}
						draw_image((col + 1) * TILE_WIDTH, row * TILE_HEIGHT, &Explosions[3].sprite[0], TILE_WIDTH, TILE_HEIGHT);
						update_screen((col + 1) * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}

					if (map[i_map] != ' ') {
						continua_esplosione = 0;
					}
				}
				if (continua_esplosione && ((col + 2) < 19)) {
					i_map = (row * 19) + (col + 2);
					if ((map[i_map] == ' ') || (map[i_map] == 'b')) {
						Explosions[4].row = row;
						Explosions[4].col = col + 2;
						Explosions[4].active = 1;
						if (map[i_map] == 'b') {
							Explosions[4].sprite = brick;
							Explosions[4].i_anim = 1;
							Explosions[4].num_anim = 7;
						} else {
							Explosions[4].sprite = err;
							Explosions[4].i_anim = 0;
							Explosions[4].num_anim = 4;
						}
						draw_image((col + 2) * TILE_WIDTH, row * TILE_HEIGHT, &Explosions[4].sprite[0], TILE_WIDTH, TILE_HEIGHT);
						update_screen((col + 2) * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}

					if (map[i_map] != ' ') {
						continua_esplosione = 0;
					}
				}
				continua_esplosione = 1;
				if (continua_esplosione && ((row - 1) >= 0)) {
					i_map = ((row - 1) * 19) + col;
					if ((map[i_map] == ' ') || (map[i_map] == 'b')) {
						Explosions[5].row = row - 1;
						Explosions[5].col = col;
						Explosions[5].active = 1;
						if (map[i_map] == 'b') {
							Explosions[5].sprite = brick;
							Explosions[5].i_anim = 1;
							Explosions[5].num_anim = 7;
						} else {
							Explosions[5].sprite = emu;
							Explosions[5].i_anim = 0;
							Explosions[5].num_anim = 4;
						}
						draw_image(col * TILE_WIDTH, (row - 1) * TILE_HEIGHT, &Explosions[5].sprite[0], TILE_WIDTH, TILE_HEIGHT);
						update_screen(col * TILE_WIDTH, (row - 1) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}

					if (map[i_map] != ' ') {
						continua_esplosione = 0;
					}
				}
				if (continua_esplosione && ((row - 2) >= 0)) {
					i_map = ((row - 2) * 19) + col;
					if ((map[i_map] == ' ') || (map[i_map] == 'b')) {
						Explosions[6].row = row - 2;
						Explosions[6].col = col;
						Explosions[6].active = 1;
						if (map[i_map] == 'b') {
							Explosions[6].sprite = brick;
							Explosions[6].i_anim = 1;
							Explosions[6].num_anim = 7;
						} else {
							Explosions[6].sprite = euu;
							Explosions[6].i_anim = 0;
							Explosions[6].num_anim = 4;
						}
						draw_image(col * TILE_WIDTH, (row - 2) * TILE_HEIGHT, &Explosions[6].sprite[0], TILE_WIDTH, TILE_HEIGHT);
						update_screen(col * TILE_WIDTH, (row - 2) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}

					if (map[i_map] != ' ') {
						continua_esplosione = 0;
					}
				}
				continua_esplosione = 1;
				if (continua_esplosione && ((row + 1) < 11)) {
					i_map = ((row + 1) * 19) + col;
					if ((map[i_map] == ' ') || (map[i_map] == 'b')) {
						Explosions[7].row = row + 1;
						Explosions[7].col = col;
						Explosions[7].active = 1;
						if (map[i_map] == 'b') {
							Explosions[7].sprite = brick;
							Explosions[7].i_anim = 1;
							Explosions[7].num_anim = 7;
						} else {
							Explosions[7].sprite = emd;
							Explosions[7].i_anim = 0;
							Explosions[7].num_anim = 4;
						}
						draw_image(col * TILE_WIDTH, (row + 1) * TILE_HEIGHT, &Explosions[7].sprite[0], TILE_WIDTH, TILE_HEIGHT);
						update_screen(col * TILE_WIDTH, (row + 1) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}

					if (map[i_map] != ' ') {
						continua_esplosione = 0;
					}
				}
				if (continua_esplosione && ((row + 2) < 11)) {
					i_map = ((row + 2) * 19) + col;
					if ((map[i_map] == ' ') || (map[i_map] == 'b')) {
						Explosions[8].row = row + 2;
						Explosions[8].col = col;
						Explosions[8].active = 1;
						if (map[i_map] == 'b') {
							Explosions[8].sprite = brick;
							Explosions[8].i_anim = 1;
							Explosions[8].num_anim = 7;
						} else {
							Explosions[8].sprite = edd;
							Explosions[8].i_anim = 0;
							Explosions[8].num_anim = 4;
						}
						draw_image(col * TILE_WIDTH, (row + 2) * TILE_HEIGHT, &Explosions[8].sprite[0], TILE_WIDTH, TILE_HEIGHT);
						update_screen(col * TILE_WIDTH, (row + 2) * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
					}

					if (map[i_map] != ' ') {
						continua_esplosione = 0;
					}
				}
			} else {
				draw_image(col * TILE_WIDTH, row * TILE_HEIGHT, &bomb[indexes[i_anim_bomb] * SPRITE_SIZE], TILE_WIDTH, TILE_HEIGHT);
				update_screen(col * TILE_WIDTH, row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
			}
		}

		read_image(x, y, TmpSprite, TILE_WIDTH, TILE_HEIGHT);
		draw_image(x, y, &pg[indexes[i_anim_pg] * SPRITE_SIZE], TILE_WIDTH, TILE_HEIGHT);

		rect_union(&redraw_rect, &old_player_rect, &player_rect);
		update_screen(redraw_rect.x, redraw_rect.y, redraw_rect.w, redraw_rect.h);
	}

	restore_keyboard();
	restore_timer();
	restore_beeper();

	set_video_mode(old_mode);

	free_resources();

	return 0;
}

static void free_resources(void) {
	free_resource((char**)&Music);
	free_resource(&walk_up);
	free_resource(&walk_down);
	free_resource(&walk_left);
	free_resource(&walk_right);
	free_resource(&wall);
	free_resource(&brick);
	free_resource(&bomb);
	free_resource(&euu);
	free_resource(&emu);
	free_resource(&ell);
	free_resource(&eml);
	free_resource(&emm);
	free_resource(&emr);
	free_resource(&err);
	free_resource(&emd);
	free_resource(&edd);
}

