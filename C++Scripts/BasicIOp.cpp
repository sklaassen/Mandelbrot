#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cstdlib>

using namespace std;
const unsigned int width = 1920;
const unsigned int height = 1080;
char fileName[] = ".ppm";

double w = 5;
double h = 0;

double xmin;
double ymin;

double xmax;
double ymax;

double dx;
double dy;

double centerx = -0.7615606563016574;
double centery = -0.08475575165077234;

int maxits = 1000;

int stripes = 5;

double dtime()
{
    double tseconds = 0.;
    struct timeval mytime;

    gettimeofday(&mytime, (struct timezone*)0);
    tseconds = (double) (mytime.tv_sec + mytime.tv_usec * 1.e-06);
    return tseconds;
}

int toRGB(double h, double s, double v, char col[3])
{
    double      hh, p, q, t, ff;
    long        i;

    if(s <= 0.)
    {
        col[0] = v;
        col[1] = v;
        col[2] = v;
        return 0;
    }
    hh = h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));
    switch(i)
    {
    case 0:
        col[0] = v*255;
        col[1] = t*255;
        col[2] = p*255;
        break;
    case 1:
        col[0] = q*255;
        col[1] = v*255;
        col[2] = p*255;
        break;
    case 2:
        col[0] = p*255;
        col[1] = v*255;
        col[2] = t*255;
        break;

    case 3:
        col[0] = p*255;
        col[1] = q*255;
        col[2] = v*255;
        break;
    case 4:
        col[0] = t*255;
        col[1] = p*255;
        col[2] = v*255;
        break;
    case 5:
    default:
        col[0] = v*255;
        col[1] = p*255;
        col[2] = q*255;
        break;
    }
    return 0;
}

int mandelbrought(double y, double x,char color[3] )
{
    int n = 0;
    double a = x;
    double b = y;
    double lasta = x;
    double lastb = y;
    double orbitCount = 0;

    while(n<maxits)
    {
        double aa = a*a;
        double bb = b*b;
        double twoab = a*b*2;
        a= aa-bb+x;
        b = twoab+y;
        if(aa+bb>10000)
        {
            //toRGB(sqrt(((double)n)/maxits)*360.,.50,.90,color);
            double lastOrbit = (double)(0.5+0.5*sin(stripes*atan2(lastb, lasta)));
            double smallCount = orbitCount-lastOrbit;
            orbitCount /= n;
            smallCount /= n-1;
            double frac = (double)(-1+log(2.0*log(10000))/log(2)-log(0.5*log(lasta*lasta+lastb*lastb))/log(2));

            double mix = frac*orbitCount+(1-frac)*smallCount;
            double orbitColor = mix;
            toRGB(sqrt(double(n)/maxits)*360.,.70,(orbitColor)*0.75+0.25/2.,color);
            return 0;
        }

        orbitCount += 0.5+0.5*sin(stripes*atan2(b, a));
        lasta = a;
        lastb = b;

        n++;
    }
    return 0;
}
char bitmap[height][width][3] = {{{0}}};
int main (int argc, char **argv)
{
    int zoom = 0;
    if(argc>1){
        zoom = std::atoi(argv[2]);
        maxits = std::atoi(argv[1]);
    }

    for(int i = 0; i < zoom; i++)
    {
        w -= (1.0/50.0)*w;
    }

    h = w*height/width;
    xmin = -(w/2)+centerx;
    ymin = -(h/2)+centery;
    xmax = xmin+w;
    ymax = ymin+h;
    dx = (xmax-xmin)/width;
    dy = (ymax-ymin)/height;

    double start_time = dtime();
    #pragma omp parallel for schedule(dynamic)
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            {
                char col[3] = {0,0,0};
                mandelbrought(y*dy+ymin,x*dx+xmin,col);
                bitmap[y][x][0] = col[0];
                bitmap[y][x][1] = col[1];
                bitmap[y][x][2] = col[2];
            }
        }
    }
    cout << dtime() - start_time << '\n';

    ofstream myfile;
    char fname[100];
    sprintf(fname,"pictures/%04d.ppm",zoom);
    myfile.open (fname);
    myfile << "P6\n"<<width<<" "<< height << "\n255\n";
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            for(int c = 0; c < 3; c++)
            {
                myfile << bitmap[y][x][c];
            }
        }
    }
    myfile.close();


    return 0;
}

