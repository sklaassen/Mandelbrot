#include <mpi.h>
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
char row[width*3];
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

int main (int argc, char **argv)
{
    int my_id, num_procs,source;
    int tag = 0;
    MPI_Status status;
    int message[1];
    int zoom = 0;
    if(argc>1){
        zoom = std::atoi(argv[2]);
        maxits = std::atoi(argv[1]);
    }

    for(int i = 0; i < zoom; i++)
    {
        w -= (1.0/50.0)*w;
    }
    MPI_Init(&argc,&argv);

    h = w*height/width;
    xmin = -(w/2)+centerx;
    ymin = -(h/2)+centery;
    xmax = xmin+w;
    ymax = ymin+h;
    dx = (xmax-xmin)/width;
    dy = (ymax-ymin)/height;

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_id);

    if(my_id != 0)
    {
        int y = 0;
        while(true){
            MPI_Recv(message,1,MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
            y=message[0];
	    if(y==-1){
	    	return 0;
	    }
            for(int x = 0; x < width; x++)
            {
                char col[3] = {0,0,0};
                mandelbrought(y*dy+ymin,x*dx+xmin,col);
                row[x*3+0] = col[0];
                row[x*3+1] = col[1];
                row[x*3+2] = col[2];

            }
            MPI_Send(row, width*3, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
        }

    }
    else
    {
        double start_time = dtime();
        char bitmap[height][width][3];
        int rows = 0;
        int openrows = num_procs-1;
        int opendest[openrows];
        //initialise free nodes
        for(int i = 0; i < openrows; i++)
        {
            opendest[i]=-1;
        }
        while(rows < height)
        {
            //if there are any free nodes
            if(openrows>0)
            {
                --openrows;
                for(int i = 0; i < num_procs-1; ++i)
                {
                    if(opendest[i]==-1)
                    {
                        opendest[i]=rows;
                        message[0] = rows;
                        MPI_Send(message, 1, MPI_DOUBLE, i+1, tag, MPI_COMM_WORLD);
                        ++rows;
                        i = num_procs-1;
                    }
                }
            }
            //listen for incoming data
            else{
                MPI_Recv(row,width*3,MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
                for(int i = 0; i < width; i++){
                    bitmap[opendest[status.MPI_SOURCE-1]][i][0] = row[i*3+0];
                    bitmap[opendest[status.MPI_SOURCE-1]][i][1] = row[i*3+1];
                    bitmap[opendest[status.MPI_SOURCE-1]][i][2] = row[i*3+2];
                }
                opendest[status.MPI_SOURCE-1]=-1;
                ++openrows;
            }

        }
        //take in remaning nodes
        while(openrows!=num_procs-1){
            MPI_Recv(row,width*3,MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            for(int i = 0; i < width; i++){
                bitmap[opendest[status.MPI_SOURCE-1]][i][0] = row[i*3+0];
                bitmap[opendest[status.MPI_SOURCE-1]][i][1] = row[i*3+1];
                bitmap[opendest[status.MPI_SOURCE-1]][i][2] = row[i*3+2];
            }
            ++openrows;
        }
        cout << dtime() - start_time << '\n';
		
		//telling other nodes to stop
		for(int i = 1; i < num_procs; ++i)
        {
        	message[0] = -1;
        	MPI_Send(message, 1, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
        }
		//image is saved to here. No need to duplicate files so not brought over from other version

    }
      MPI_Finalize();

    return 0;
}

