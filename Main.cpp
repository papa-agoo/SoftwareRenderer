#include "SDL.h"
#include "Rasterizer.h"

/*
* Set the pixel at (x, y) to the given value
* NOTE: The surface must be locked before calling this!
*/
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;

	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		} else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *)p = pixel;
		break;
	}
}

class PixelShader : public PixelShaderBase<PixelShader> {
public:
	static SDL_Surface* surface;

	static void drawPixel(const PixelData &p)
	{
		int rint = (int)(p.r * 255);
		int gint = (int)(p.g * 255);
		int bint = (int)(p.b * 255);
		Uint32 color = SDL_MapRGB(surface->format, rint, gint, bint);
		putpixel(surface, (int)p.x, (int)p.y, color);
	}
};

SDL_Surface* PixelShader::surface;

Vertex randomVertex()
{
	Vertex v;
	
	v.x = (float)(rand() % 640);
	v.y = (float)(rand() % 480);

	v.r = (float)rand() / RAND_MAX;
	v.g = (float)rand() / RAND_MAX;
	v.b = (float)rand() / RAND_MAX;

	return v;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(
		"Software Renderer",
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED,
		640, 
		480,
		0
	);

	SDL_Surface *screen = SDL_GetWindowSurface(window);

	srand(1234);

    Rasterizer r;
    r.setScissorRect(0, 0, 640, 480);
	PixelShader::surface = screen;

	Uint32 start = SDL_GetTicks();

	for (int i = 0; i < 10000; ++i)
	{
		Vertex v0 = randomVertex();
		Vertex v1 = randomVertex();
		Vertex v2 = randomVertex();

		r.drawTriangle<PixelShader>(v0, v1, v2);
	}

	Vertex v0, v1, v2;

	v0.x = 320;
	v0.y = 100;
	v0.r = 1.0;
	v0.g = 0.0;
	v0.b = 0.0;

	v2.x = 180;
	v2.y = 200;
	v2.r = 0.0;
	v2.g = 1.0;
	v2.b = 0.0;

	v1.x = 480;
	v1.y = 300;
	v1.r = 0.0;
	v1.g = 0.0;
	v1.b = 1.0;

	r.drawTriangle<PixelShader>(v0, v1, v2);

	Uint32 end = SDL_GetTicks();
	printf("%i\n", end - start);

    SDL_UpdateWindowSurface(window);

	SDL_Delay(3000);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
