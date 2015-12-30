/*	Julia is a program that draws julia sets.
    Copyright (C) <2015>  <Aladin Virmaux>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*  TODO: *     - rendre propre la partie mouvements SDL, duplication de code
 *              - choix à l'utisateur pour les couleurs
 *              - ne pas tout recaluler à chaque mouvement
 */
#include "stdlib.h"
#include "math.h"
#include "SDL/SDL.h"
#include <getopt.h>

#define program_name "julia"

typedef struct {
    double xmin;
    double xmax;
    double ymin;
    double ymax;
} grid;

static char const short_options[] = "h:i:s:u:w:z:";
static struct option const long_options[] =
{
    {"iter", required_argument, NULL, 'i'},
    {"height", required_argument, NULL, 'h'},
    {"smooth", required_argument, NULL, 's'},
    {"unzoom", required_argument, NULL, 'u'},
    {"width", required_argument, NULL, 'w'},
    {"xmin", required_argument, NULL, 2},
    {"xmax", required_argument, NULL, 3},
    {"ymin", required_argument, NULL, 4},
    {"ymax", required_argument, NULL, 5},
    {"zoom", required_argument, NULL, 'z'},
    {"help", no_argument, NULL, 1},
    {NULL, 0, NULL, 0}
};

int usage(int status)
{
    fputs("Usage: julia [OPTIONS...] [-- a b]\n\n", stdout);
    fputs("Draw the Julia set associated with the complex number c = a + ib.\n", stdout);
    fputs("\n\
  -i, --iter=int            number of iterations, default is 255\n\
  -s, --smooth=float        smoothness of the movements in %, default is 8%\n\
  -u, --unzoom=float        coefficient of unzoom, default is 2\n\
  -z, --zoom=float          coefficient of zoom, default is φ\n\
  --help                    display this help and exit\n", stdout);
    fputs("\n\
  Window:\n\
  -h, --height=int          height of the screen in pixels, default is 900\n\
  -w, --width=int           width of the screen in pixels, default is 1400\n\n\
  --xmin=float              default is -1.4\n\
  --xmax=float              default is 1.4\n\
  --ymin=float              default is -1.7\n\
  --ymax=float              default is 1.7\n", stdout);
    fputs("\n\
  Key binding:\n\
  Left click / +            zoom +\n\
  Right click / -           zoom -\n\
  Arrow keys                move\n\
  q                         quit\n", stdout);
    exit (status);
}

int mandelbrot(float a, float b, int n) {
    int cpt = 0;
    float tmp, x = 0, y = 0;
    while (x*x + y*y < 4 && cpt++ < n) {
        tmp = x;
        x = x*x - y*y + a;
        y  = 2 * tmp * y + b;
    }
    return (cpt < n)? cpt : 0;
}

int julia(double x, double y, double a, double b, int n) {
    int cpt = 0;
    double tmp;
    while (x*x + y*y < 4 && cpt++ < n) {
        tmp = x;
        x = x*x - y*y + a;
        y  = 2 * tmp * y + b;
    }
    return (cpt < n)? cpt : 0;
}

/* Julia set with z³ + c
 * Play with it!
 */
int julia3(double x, double y, double a, double b, int n) {
    int cpt = 0;
    double tmp;
    while (x*x + y*y < 4 && cpt++ < n) {
        tmp = x;
        x = x*x*x - 3*x*y*y + a;
        y  = 3 * tmp * tmp * y - y * y * y + b;
    }
    return (cpt < n)? cpt : 0;
}

void fractal_draw(SDL_Surface *sf, double cx, double cy, grid g, int n_iter) {
    int x, y, c;
    double a, b;
    for (x = 0; x < sf->w; x++) {
        for (y = 0; y < sf->h; y++) {
            /* grid -> plot */
            a = ((double) x / sf->w) * (g.xmax - g.xmin) + g.xmin;
            b = ((double) y / sf->h) * (g.ymax - g.ymin) + g.ymin;

            c = julia(a, b, cx, cy, n_iter);
            /* c = julia3(a, b, cx, cy, n_iter); */
            /* Black and white*/
            /* ((Uint32*) sf->pixels)[y*sf->w + x] = SDL_MapRGB(sf -> format, c, c, c); */
            /* Weird colord */
            ((Uint32*) sf->pixels)[y*sf->w + x] = SDL_MapRGB(sf -> format,
                (1 + sin(1.5+c*0.35)) * 127, (1 + cos(c*0.12)) * 127, (1 + sin(c*0.6)) * 127);
            /* Bleuet */
            /* ((Uint32*) sf->pixels)[y*sf->w + x] = SDL_MapRGB(sf -> format, */
            /*     c, c, (1 + sin(c * 0.2)) * 17); */

            /* Chaos */
            /* ((Uint32*) sf->pixels)[y*sf->w + x] = SDL_MapRGB(sf -> format, */
            /*     (1 + cos(c*c*c*c)) * 127., (1 + sin(c*c)) * 127., (1 + cos(3*c*c+1)) * 127.); */
            /* ((Uint32*) sf->pixels)[y*sf->w + x] = SDL_MapRGB(sf -> format, 206+8*c, 20+2*c, 226-4*c); */
        }
    }

    SDL_UpdateRect(sf, 0, 0, 0, 0);
}

/* Main
 *
 * Initialisation SDL,
 * Quitter
 */
int main(int argc, char ** argv)
{
    /*Parse options and get arguments */

    int optc;
    int n_iter = 255;
    int height = 900, width = 1400;
    float zoom = (sqrt(5) + 1)/2, unzoom = 2, smooth=8.0;
    grid g = {-1.4, 1.4, -1.7, 1.7};
    double cx, cy;
    while ((optc = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (optc) {
            case 1:
                usage(EXIT_SUCCESS);
                break;
            case 2:
                g.xmin = strtof(optarg, NULL);
                break;
            case 3:
                g.xmax = strtof(optarg, NULL);
                break;
            case 4:
                g.ymin = strtof(optarg, NULL);
                break;
            case 5:
                g.ymax = strtof(optarg, NULL);
                break;
            case 'i':
                n_iter = atoi(optarg);
                break;
            case 'h':
                height = atoi(optarg);
            case 's':
                smooth = strtof(optarg, NULL);
                printf("%f\n", smooth);
                break;
            case 'u':
                unzoom = strtof(optarg, NULL);
                break;
            case 'w':
                width = atoi(optarg);
            case 'z':
                zoom = strtof(optarg, NULL);
                break;
            case '?':
                break;
            default:
                usage(EXIT_FAILURE);
        }
    }
    /* Part remaining, if more than two arguments error*/
    if (optind == argc) {
        cx = -0.34;
        cy = 0.71;
    }
    else if (optind + 2 == argc) {
        cx = strtod(argv[optind++], NULL);
        cy = strtod(argv[optind++], NULL);
    }
    else {
        usage(EXIT_FAILURE);
    }

    /* Mise en place SDL */
    SDL_Event event;
    SDL_Surface *surface = NULL;
    SDL_Init(SDL_INIT_VIDEO);
    surface = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE);
    if (surface == NULL) {
        fputs("Fail to open SDL Video", stderr);
        return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("Julia", NULL);

    /* Dessine la fractale */
    double m_x, m_y;
    fractal_draw(surface, cx, cy, g, n_iter);

    while(1) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:

                    switch (event.key.keysym.sym) {

                        case SDLK_EQUALS:
                            m_x = (g.xmax + g.xmin) / 2;
                            m_y = (g.ymax + g.ymin) / 2;
                            g.xmin = m_x - (g.xmax - g.xmin) / (2 * zoom);
                            g.xmax = m_x + (g.xmax - g.xmin) / (2 * zoom);
                            g.ymin = m_y - (g.ymax - g.ymin) / (2 * zoom);
                            g.ymax = m_y + (g.ymax - g.ymin) / (2 * zoom);
                            fractal_draw(surface, cx, cy, g, n_iter);
                            break;

                        case SDLK_MINUS:
                            m_x = (g.xmax + g.xmin) / 2;
                            m_y = (g.ymax + g.ymin) / 2;
                            g.xmin = m_x - unzoom*(m_x - g.xmin);
                            g.xmax = m_x - unzoom*(m_x - g.xmax);
                            g.ymin = m_y - unzoom*(m_y - g.ymin);
                            g.ymax = m_y - unzoom*(m_y - g.ymax);
                            fractal_draw(surface, cx, cy, g, n_iter);
                            break;

                        case SDLK_DOWN:
                            m_y = (g.ymax - g.ymin) * (smooth/100);
                            g.ymin += m_y;
                            g.ymax += m_y;
                            fractal_draw(surface, cx, cy, g, n_iter);
                            break;

                        case SDLK_UP:
                            m_y = (g.ymax - g.ymin) * (smooth/100);
                            g.ymin -= m_y;
                            g.ymax -= m_y;
                            fractal_draw(surface, cx, cy, g, n_iter);
                            break;

                        case SDLK_RIGHT:
                            m_x = (g.xmax - g.xmin) * (smooth/100);
                            g.xmin += m_x;
                            g.xmax += m_x;
                            fractal_draw(surface, cx, cy, g, n_iter);
                            break;

                        case SDLK_LEFT:
                            m_x = (g.xmax - g.xmin) * (smooth/100);
                            g.xmin -= m_x;
                            g.xmax -= m_x;
                            fractal_draw(surface, cx, cy, g, n_iter);
                            break;

                        case SDLK_q:
                            exit (EXIT_SUCCESS);
                            break;

                        default:
                            break;
                    }

                    break;

                case SDL_MOUSEBUTTONDOWN:

                    switch(event.button.button) {
                        case SDL_BUTTON_LEFT:
                            m_x = ((double) event.button.x / surface->w) * (g.xmax - g.xmin) + g.xmin;
                            m_y = ((double) event.button.y / surface->h) * (g.ymax - g.ymin) + g.ymin;
                            g.xmin = m_x - (g.xmax - g.xmin) / (2 * zoom);
                            g.xmax = m_x + (g.xmax - g.xmin) / (2 * zoom);
                            g.ymin = m_y - (g.ymax - g.ymin) / (2 * zoom);
                            g.ymax = m_y + (g.ymax - g.ymin) / (2 * zoom);
                            fractal_draw(surface, cx, cy, g, n_iter);
                            break;

                        case SDL_BUTTON_RIGHT:
                            m_x = ((double) event.button.x / surface->w) * (g.xmax - g.xmin) + g.xmin;
                            m_y = ((double) event.button.y / surface->h) * (g.ymax - g.ymin) + g.ymin;
                            g.xmin = m_x - unzoom*(m_x - g.xmin);
                            g.xmax = m_x - unzoom*(m_x - g.xmax);
                            g.ymin = m_y - unzoom*(m_y - g.ymin);
                            g.ymax = m_y - unzoom*(m_y - g.ymax);
                            fractal_draw(surface, cx, cy, g, n_iter);
                            break;

                        default:
                            break;
                    }

                    break;

                case SDL_QUIT:
                    exit (EXIT_SUCCESS);
                    break;
                default:
                    break;
            }
        }
    }


    /* Cleaning */
    SDL_Quit();
    return EXIT_SUCCESS;
}
