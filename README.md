# julia
Draw Julia sets with the [SDL](https://www.libsdl.org/) library

    Usage: julia [OPTIONS...] [-- a b]

    Draw the Julia set associated with the complex number c = a + ib.

      -i, --iter=int            number of iterations, default is 255
      -s, --smooth=float        smoothness of the movements in %, default is 8%
      -u, --unzoom=float        coefficient of unzoom, default is 2
      -z, --zoom=float          coefficient of zoom, default is φ
      --help                    display this help and exit

      Window:
      -h, --height=int          height of the screen in pixels, default is 900
      -w, --width=int           width of the screen in pixels, default is 1400

      --xmin=float              default is -1.4
      --xmax=float              default is 1.4
      --ymin=float              default is -1.7
      --ymax=float              default is 1.7

      Key binding:
      Left clic / +             zoom +
      Right clic / -            zoom -
      Arrow keys                move
      q                         quit
