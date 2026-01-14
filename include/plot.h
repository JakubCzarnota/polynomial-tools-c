#ifndef PLOT_H
#define PLOT_H

typedef double (*Function)(double x);

// Plots y = f(x) into an RGB buffer
void plot_function(
    unsigned char *image,
    int width,
    int height,
    Function f,
    double xmin,
    double xmax,
    double ymin,
    double ymax,
    int thickness
);

#endif // PLOT_H
