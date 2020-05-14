#ifndef COMPLEX_NUM_H
#define COMPLEX_NUM_H
typedef struct _complexnum {
    double r;
    double i;
} complexnum;

#define real(r) ((complexnum){r, 0})
#define imag(i) ((complexnum){0, i})

complexnum add_complex(complexnum c, complexnum d) {
    complexnum e;
    e.r = c.r + d.r;
    e.i = c.i + d.i;
    return e;
}

complexnum multiply_complex(complexnum c, complexnum d) {
    complexnum e;
    e.r = (c.r * d.r) - (c.i * d.i);
    e.i = (c.r * d.i) + (c.i * d.r);
    return e;
}

complexnum pow_complex(complexnum c, int e) {
    complexnum d = real(1);
    int i;
    for(i = 0; i < e; i++)
        d = multiply_complex(d, c);
    return d;
}

char *show_complex(complexnum c) {
    char *a = malloc((5 + (1<<8)) * sizeof(char));
    if(c.i != 0)
        sprintf(a, "%7.2f + %5.2fi", c.r, c.i);
    else
        sprintf(a, "%15.2f", c.r);
    return a;
}
#endif