#include <raylib.h>
#define MOUSE_SIZE 32

const char *mouse_ascii = "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"
                          "GGGGGGGBBBBGGGGGGGGGGBBBBGGGGGGG"
                          "GGGGGGBPPPPBGGGGGGGGBPPPPBGGGGGG"
                          "GGGGGBPPPPPPBGGGGGGBPPPPPPBGGGGG"
                          "GGGGBPPPPPPPPBGGGGBPPPPPPPPBGGGG"
                          "GGGGBPPPPPPPPBGGGGBPPPPPPPPBGGGG"
                          "GGGGBPPPPMMMMBGGGGBMMMMPPPPBGGGG"
                          "GGGGBPPPMMMMMBGGGGBMMMMMPPPBGGGG"
                          "GGGGBPPMMMMMMBGGGGBMMMMMMPPBGGGG"
                          "GGGGBYMMMMMMYBYYYYBYMMMMMMYBGGGG"
                          "GGGGGBLLLLLLLLLLLLLLLLLLLLBGGGGG"
                          "GGGGBLLLLLLLLLLLLLLLLLLLLLLBGGGG"
                          "GGGBLLLLLLLLLLLLLLLLLLLLLLLLBGGG"
                          "GGBYYYYYLLLLLLLLLLLLLLLLYYYYYBGG"
                          "GGBYYYYYYYLLLLLLLLLLLLYYYYYYYBGG"
                          "GBYYYYYYYYYYYYYYYYYYYYYYYYYYYYBG"
                          "GBYYYYYYYYYYYYYYYYYYYYYYYYYYYYBG"
                          "GBYYYDDDDDDDDYYYYYYDDDDDDDDYYYBG"
                          "GBYYYYWWWWWWYYYYYYYYWWWWWWYYYYBG"
                          "GBYYYWWBWBBWWYYYYYYWWBBWBWWYYYBG"
                          "GBYYYWWBBBBWWYYYYYYWWBBBBWWYYYBG"
                          "GBYYYWWBBBBWWYYYYYYWWBBBBWWYYYBG"
                          "GBYYYYWWWWWWYYYYYYYYWWWWWWYYYYBG"
                          "GBYYYYDDDDDDYYYYYYYYDDDDDDYYYYBG"
                          "GGBYYYYYYYYYYBPPPPBYYYYYYYYYYBGG"
                          "BBBYYYYYYYYYBPPPPPPBYYYYYYYYYBBB"
                          "GGBYYYYYYYYYBMMMMMMBYYYYYYYYYBGG"
                          "BBBYYYYYYYYYYBMMMMBYYYYYYYYYYBBB"
                          "GGGBYYYYYYYYYYYBBYYYYYYYYYYYBGGG"
                          "GGGBYYYYYYBBBBMMMMBBBBYYYYYYBGGG"
                          "GGGGBDDDDDDDDBBBBBBDDDDDDDDBGGGG"
                          "GGGGGGGBBBBBBBBBBBBBBBBBBGGGGGGG";

static Color map_colors(char chr) {
  switch (chr) {
    case 'B':
      return BLACK;
    case 'Y':
      return GRAY;
    case 'L':
      return LIGHTGRAY;
    case 'D':
      return DARKGRAY;
    case 'P':
      return PINK;
    case 'M':
      return MAROON;
    case 'W':
      return WHITE;
    default:
      return BLANK;
  }
}

void draw_mouse(Vector2 pos, int size) {
  float pixel_size = (float)size / MOUSE_SIZE;
  for (int y = 0; y < MOUSE_SIZE; y++){
    for (int x = 0; x < MOUSE_SIZE; x++) {
      float py = pos.y + y * pixel_size;
      float px = pos.x + x * pixel_size;
      Color color = map_colors(mouse_ascii[y * MOUSE_SIZE + x]);
      DrawRectangleV((Vector2){px, py}, (Vector2){pixel_size, pixel_size}, color);
    }
  }
}