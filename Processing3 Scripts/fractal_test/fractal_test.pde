import processing.video.*;
import java.io.File;
import java.io.IOException;

void setup() {
  size(1080, 720);
  colorMode(HSB, 1);
  zoom = 5;
  lastpoint = -0.761574;
  lastpointy = -0.0847596;
  logBase = 1.0 / Math.log(2.0);
  logHalfBase = Math.log(0.5)*logBase;
}

double zoom;
double lastpoint;
double lastpointy;
float angle = 0.3;

double mx;
double my;

double aa;
double bb;
double twoab;
int dmi = 10;
int maxiterations=dmi;


////////////////stripes//////////////
float stripes = 5.0;
double logBase;
double logHalfBase;


void mouseWheel(MouseEvent event) {
  zoom += event.getCount()/(double)9*zoom;
  //println(event.getCount());
}
//move around and alter iteration count using key presses
void keyPressed() {
if (key == CODED) {
    if (keyCode == LEFT) {
      lastpoint-=zoom/100;
    } else if (keyCode == RIGHT) {
      lastpoint+=zoom/100;
    }
    else if (keyCode == DOWN) {
      lastpointy+=zoom/100;
    } else if (keyCode == UP) {
      lastpointy-=zoom/100;
    } 
     else if (keyCode == CONTROL) {
      maxiterations+=10;
    }      else if (keyCode == SHIFT) {
      maxiterations-=10;
    } 
    else if(keyCode == ALT){
      angle+=0.01;
    }
  }
}

//main meathod that draw to the screen
void draw() {
  background(255);
  
  //float ca = 0;
  //float cb = 0;

  double w = zoom;
  double h = w*height/width;
  
  double xmin = -(zoom/2)+(lastpoint);
  double ymin = -(h/2)+lastpointy;

  loadPixels();

  //calculating th ratio for the pixel
  double xmax = xmin+w;
  double ymax = ymin+h;
  double dx = (xmax-xmin)/width;
  double dy = (ymax-ymin)/height;

  double y = ymin;
  for (int j = 0; j < height; j++) {
    // Start x
    double x = xmin;
    for (int i = 0; i < width; i++) {

      // Now we test, as we iterate z = z^2 + cm does z tend towards infinity?
      double a = x;
      double b = y;
      //double lasta = x;
      //double lastb = y;
      //float orbitCount = 0;
      
      int n = 0;
      while (n < maxiterations) {
        double aa = a*a;
        double bb = b*b;
        double twoab = 2*a*b;
        
        a = aa-bb+x;
        b = twoab+y;
        // Infinty in our finite world is simple, let's just consider it 16
        if (aa+bb > 10000)
        {
          break;  // Bail
        }
        
        //orbitCount += 0.5+0.5*Math.sin(stripes*Math.atan2(b, a));
        //lasta = a;
        //lastb = b;
        n++;
      }
      if (n == maxiterations) {
        pixels[i+j*width] = color(0);
      } else {

        pixels[i+j*width] = color(sqrt(float(n) / maxiterations),.5,.90);
        //pixels[i+j*width] = color((float(n)*(n/360+1.)%360./360.),.5,.90);
        //float smoothed = (float)(1 + n - Math.log(Math.log(Math.sqrt(a*a+b*b)))/Math.log(2.0));
        //pixels[i+j*width] = colorI;//color(float(n)/maxiterations*smooth,.6,.9);
        /*
        float lastOrbit = (float)(0.5+0.5*Math.sin(stripes*Math.atan2(lastb, lasta)));
        float smallCount = orbitCount-lastOrbit;
        orbitCount /= n;
        smallCount /= n-1;
        float frac = (float)(-1+Math.log(2.0*Math.log(10000))/Math.log(2)-Math.log(0.5*Math.log(lasta*lasta+lastb*lastb))/Math.log(2));      

        float mix = frac*orbitCount+(1-frac)*smallCount;
        float orbitColor = mix;
        pixels[i+j*width] = color(float(n)/maxiterations,.6,(orbitColor)*0.5+0.5/2);
        */

      }
      x += dx;
    }
    y += dy;
  }
  updatePixels();
  
  //used to save frames to test video generation speeds and if zoom is too fast
  //saveFrame("output/fractal_####.png");
  //zoom += -1/100.*zoom;
  //maxiterations++;
  
  //center of the screen to know where zooming towards
  //stroke(100,100,100);
  //point(width/2,height/2);
  
  //debugging help
  //println(frameRate + " : " + zoom + " : " + maxiterations + " |X: "+lastpoint + " Y: "+lastpointy);
}