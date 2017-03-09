#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h> // for usleep
#include "gfx.h"
#include "bodies.h"


// define constants
#define NUM_THREADS 1


// define globals
double           timeStep = 100; //all units are given in base SI
int              numBodies = 3;
pthread_mutex_t  mutex;
body            *bodies;
vector3D        *forces;


// define structs
typedef struct pair {
  int a, b;
} pair;


// forward declarations
pair getNextBodySet(int reset);
int getNextBody(int reset);
void clearForces();
void* updateForces();
void* updatePosAndVels();


/* Main method
 * 
 * @author Ethan Brummel, Garth Van Donselaar, Jason Vander Woude
 */
int main () {
  pair p;
  int b;
  //initialize mutex
  pthread_mutex_init(&mutex, NULL);
  
  //get memory space
  //TODO: handle error condition
  bodies = (body*)malloc(sizeof(body) * numBodies);
  forces = (vector3D*)malloc(sizeof(vector3D) * numBodies);
  
  //set window size
  int win_x_size = 1200;
  int win_y_size = 800;
  
  bodies[0].pos.x = 0;
  bodies[0].pos.y = 0;
  bodies[0].pos.z = 0;
  bodies[0].vel.x = 0;
  bodies[0].vel.y = 0;
  bodies[0].vel.z = 0;
  bodies[0].mass = 200000000;
  bodies[0].density = 1;
  
  bodies[1].pos.x = 142;
  bodies[1].pos.y = 0;
  bodies[1].pos.z = 0;
  bodies[1].vel.x = 0;
  bodies[1].vel.y = -0.01;
  bodies[1].vel.z = 0;
  bodies[1].mass = 10000000;
  bodies[1].density = 1;
  
  bodies[2].pos.x = -142;
  bodies[2].pos.y = 0;
  bodies[2].pos.z = 0;
  bodies[2].vel.x = 0;
  bodies[2].vel.y = 0.011;
  bodies[2].vel.z = 0;
  bodies[2].mass = 10000000;
  bodies[2].density = 1;
  
//   printf("1:\n");
//   for (int i=0; i<numBodies; ++i) {
//     printf("p:<%f, %f, %f>\t\tv:<%f, %f, %f>\n", bodies[i].pos.x, bodies[i].pos.y, bodies[i].pos.z, 
// 	                                     bodies[i].vel.x, bodies[i].vel.y, bodies[i].vel.z);
//     fflush(stdout);
//   }
  
  
//   //printf("-1\n"); fflush(stdout);
//   getNextBodySet(1);
//   //printf("-1.5\n"); fflush(stdout);
//   for (int i=0; i<10; ++i) {
//     p = getNextBodySet(0);
//     printf("(%d, %d)", p.a, p.b);
//   }  
//   getNextBodySet(1);
//   for (int i=0; i<10; ++i) {
//     p = getNextBodySet(0);
//     printf("(%d, %d)", p.a, p.b);
//   }
//   
//   printf("\n");
//   b = getNextBody(1);
//   for (int i=0; i<10; ++i) {
//     b = getNextBody(0);
//     printf("%d, ",b);
//   }
//   getNextBody(1);
//   for (int i=0; i<10; ++i) {
//     b = getNextBody(0);
//     printf("%d, ",b);
//   }
//   printf("\n\n----------\n\n");
  
  
  // Open a new window for drawing.
  gfx_open(win_x_size, win_y_size, "N-Body");
  int iter = 0;
  while (iter < 5000) {
    ++iter;
  
    printf("%d:\n",iter);
    for (int i=0; i<numBodies; ++i) {
      printf("p:<%f, %f, %f>\t\tv:<%f, %f, %f>\t\tf:<%f, %f, %f>\n", 
	     bodies[i].pos.x, bodies[i].pos.y, bodies[i].pos.z,
	     bodies[i].vel.x, bodies[i].vel.y, bodies[i].vel.z, 
	     forces[i].x, forces[i].y, forces[i].z);
      fflush(stdout);
    }
     
    // set all forces to 0
    clearForces();

    // initialize all threads
    pthread_t threads[NUM_THREADS];
    for (int t=0; t<NUM_THREADS; ++t) {
      pthread_create(&(threads[t]), NULL, updateForces, NULL);
    }

    // block on thread completion
    for (int t=0; t<NUM_THREADS; ++t) {
      pthread_join(threads[t], NULL);
    }

    // initialize all threads
    for (int t=0; t<NUM_THREADS; ++t) {
      pthread_create(&(threads[t]), NULL, updatePosAndVels, NULL);
    }

    // block on thread completion
    for (int t=0; t<NUM_THREADS; ++t) {
      pthread_join(threads[t], NULL);
    }
    
    gfx_clear();
    // Set the current drawing color to green.
    gfx_color(0,200,100);
    for (int i=0; i<numBodies; ++i) {
      gfx_circle(bodies[i].pos.x + win_x_size/2, bodies[i].pos.y + win_y_size/2, 50);
    }
    gfx_flush();
    usleep(10000);
    
     
  } //END: while (1)
  
  
//   // Open a new window for drawing.
//   gfx_open(win_x_size, win_y_size, "N-Body");  
//   
//   while (1) {
//     // set all forces to 0
//     clearForces();
// 
//     // initialize all threads
//     pthread_t threads[NUM_THREADS];
//     for (int t=0; t<NUM_THREADS; ++t) {
//       pthread_create(&(threads[t]), NULL, updateForces, NULL);
//     }
// 
//     // block on thread completion
//     for (int t=0; t<NUM_THREADS; ++t) {
//       pthread_join(threads[t], NULL);
//     }
//     
//     // initialize all threads
//     for (int t=0; t<NUM_THREADS; ++t) {
//       pthread_create(&(threads[t]), NULL, updatePosAndVels, NULL);
//     }
// 
//     // block on thread completion
//     for (int t=0; t<NUM_THREADS; ++t) {
//       pthread_join(threads[t], NULL);
//     } 
//     
//     gfx_clear();
//     // Set the current drawing color to green.
//     gfx_color(0,200,100);
//     for (int i=0; i<numBodies; ++i) {
//       gfx_circle(bodies[i].pos.x + win_x_size/2, bodies[i].pos.y + win_y_size/2, 50);
//     }
//     gfx_flush();
//     usleep(100000);
//   }
}


//PTHREAD COMMENTED OUT
pair getNextBodySet(int reset) {
    static int finished, i, j;
    //sprintf("0\n"); fflush(stdout);
    //guard with mutex
    //TODO: check if mutex can be locked later
    //pthread_mutex_lock(&mutex);
    //printf("0.1\n"); fflush(stdout);
    
      pair p;
    //printf("0.2\n"); fflush(stdout);
      
      //printf("1\n");
      
      if (reset) {
	finished = 0;
	i = 0;
	j = 0;
	p.a = -1;
	p.b = -1;
	return p;
      }
      //printf("2\n"); fflush(stdout);
      
      if (finished==0) {
	if (j==i) {
	  ++i;
	  j=0;
	}
	
	//store return value
	p.a = i;
	p.b = j;
	
	++j;
	
	if (i==numBodies) {
	  finished = 1;
	}
      }
      
      //printf("3\n"); fflush(stdout);
      
      if (finished) {
	p.a = -1;
	p.b = -1;
      }
      
    //release mutex
    //TODO: check if mutex can be released earlier
    //pthread_mutex_unlock(&mutex);
    
    return p;
}

//PTHREAD COMMENTED OUT
int getNextBody(int reset) {
  //pthread_mutex_lock(&mutex);
    static int b, finished = 0;
  
    if (reset) {
      finished = 0;
      b = -1;
      return b;
    }
    
    ++b;
    if (b==numBodies)
      finished = 1;
    if (finished==1)
      b = -1;
  //pthread_mutex_unlock(&mutex);
  
  return b;
}


void clearForces() {
  for (int i=0; i<numBodies; ++i) {
    forces[i].x = 0;
    forces[i].y = 0;
    forces[i].z = 0;
  }
}


void *updateForces() {
  pair p;
  
  getNextBodySet(1);
  
  while (1) {
    p = getNextBodySet(0);
    if (p.a == -1 && p.b == -1) {
      // kill this thread
      pthread_exit(NULL);
    } else {
      // compute interaction forces between body A and body B
      vector3D forceOnA = getForce(bodies[p.a], bodies[p.b]);
      vector3D forceOnB = negateVector3D(forceOnA);
      
      // add the respective forces to the running total forces
      forces[p.a] = vector3DSum(forces[p.a], forceOnA);
      forces[p.b] = vector3DSum(forces[p.b], forceOnB);
    }
  }
}


void *updatePosAndVels() {
  int x;
  
  getNextBody(1);
  
  while (1) {
    x = getNextBody(0);
    if (x == -1) {
      // kill this thread
      pthread_exit(NULL);
    } else {
      vector3D accel = getAcceleration(bodies[x], forces[x]);
      // x = x_0 + v*t - 1/2*a*t^2
      bodies[x].pos = vector3DSum(
                                  vector3DSum(
                                              bodies[x].pos, 
                                              vector3DScale(bodies[x].vel, timeStep)), 
                                  vector3DScale(accel, 0.5*timeStep*timeStep));
      // v = v_0 + a*t
      bodies[x].vel = vector3DSum(bodies[x].vel, vector3DScale(accel, timeStep));
    }
  }
}




