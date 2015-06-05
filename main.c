#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

SDL_Window* window = NULL;
SDL_GLContext context;

typedef struct 
{
float x, y;
} vec;
#define NUM_OF_OBJECTS 70 
vec positions[NUM_OF_OBJECTS];
vec velocities[NUM_OF_OBJECTS];

float t_values[NUM_OF_OBJECTS][NUM_OF_OBJECTS];


float total_time = 0.0f;
int num_of_collisions = 0;

float randFloat()
{
	float r = 1.8f * (((float)rand()/(float)RAND_MAX) -0.5f);
	return r;
}
void DrawCircle(float cx, float cy, float r, int num_segments) 
{ 
	float theta = 2 * 3.1415926 / (float)num_segments; 
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = r;//we start at angle = 0 
	float y = 0; 
    
	glBegin(GL_LINE_LOOP); 
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		glVertex2f(x + cx, y + cy);//output vertex 
        
		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	} 
	glEnd(); 
}

void recalculate_t_values()
{
	for(int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		for(int j = i + 1; j < NUM_OF_OBJECTS; j++)
		{
			vec Vab = { velocities[i].x - velocities[j].x, velocities[i].y - velocities[j].y };
			vec Pab = { positions[i].x - positions[j].x, positions[i].y - positions[j].y };

			float a = Vab.x * Vab.x + Vab.y * Vab.y;

			float b = 2 * ( Pab.x * Vab.x + Pab.y * Vab.y ); 

			float c = ( Pab.x * Pab.x + Pab.y * Pab.y ) - (4 * .1 * .1);

			float det = b * b - 4 * a * c;

			if(det > 0.0f)
			{		

				float t1 = (-b + sqrtf(det)) / ( 2 * a );
				float t2 = (-b - sqrtf(det)) / ( 2 * a );
				float tval = 0.0f;
				if(t1 < t2) { tval = t1; } else { tval = t2; }
				t_values[i][j] = tval;

			}
			else
			{
				t_values[i][j] = -1.0f;
			}
		}
	}
}


int main()
{
	int quit = 0;
	SDL_Event event;
	memset(&t_values, 0, NUM_OF_OBJECTS * NUM_OF_OBJECTS);
	for(int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		vec rand1 = {randFloat(), randFloat()};
		vec rand2 = {randFloat(), randFloat()};
		positions[i] = rand1; 
		velocities[i] = rand2;
	}

	SDL_Init( SDL_INIT_EVERYTHING );
	window = SDL_CreateWindow("My SDL Empty Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 640, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);

	if(!window)
	{
		printf("Window could not be created");
		exit(1);
	}

	context = SDL_GL_CreateContext( window );

	if(!context)
	{
		printf("Could not create context!");
		exit(1);
	}

	if( SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("WARNING: unable to set vsync");
	}

	// initialize opengl


	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	if(glGetError() != GL_NO_ERROR)
	{
		printf("OpenGL error!");
		exit(1);
	}

	glClearColor( 0.0f, 1.0f, 0.0f, 1.0f );


	while (!quit)
	{
		SDL_PollEvent(&event);

		switch(event.type)
		{
			case SDL_QUIT:
				quit = 1;
				break;
		}
		float tdelta = .22f;

		total_time += tdelta;
		recalculate_t_values();

		//find minimum t val
		float min_t_val = tdelta;
		int min_i, min_j;

		for(int i = 0; i < NUM_OF_OBJECTS; i++)
		{
			for(int j = i + 1; j < NUM_OF_OBJECTS; j++)
			{
				if(t_values[i][j] > 0.0f && t_values[i][j] < min_t_val) { min_i = i; min_j = j; min_t_val = t_values[i][j]; }	
			}
		}

		while(min_t_val < tdelta)
		{
			for(int i = 0; i < NUM_OF_OBJECTS; i++)
			{
				positions[i].x += velocities[i].x * min_t_val;
				positions[i].y += velocities[i].y * min_t_val;
			}
			tdelta -= min_t_val;
			velocities[min_i].x = .05f * (positions[min_i].x-positions[min_j].x);
			velocities[min_i].y = .05f * (positions[min_i].y-positions[min_j].y);
			velocities[min_j].x = .05f * (positions[min_j].x-positions[min_i].x);
			velocities[min_j].y = .05f * (positions[min_j].y-positions[min_i].y);
			num_of_collisions++;

			recalculate_t_values();
			min_t_val = tdelta;	
			for(int i = 0; i < NUM_OF_OBJECTS; i++)
			{
				for(int j = i + 1; j < NUM_OF_OBJECTS; j++)
				{
					if(t_values[i][j] > 0.0f && t_values[i][j] < min_t_val) { min_i = i; min_j = j; min_t_val = t_values[i][j]; }	
				}
			}
		}
		//step to tval
		//go until minimum t val is greater than frame time and advance until then
		//update positions
		for(int i = 0; i < NUM_OF_OBJECTS; i++)
		{
			if(positions[i].x < .9f) { velocities[i].x = -velocities[i].x; }
			if(positions[i].x > -.9f) { velocities[i].x = -velocities[i].x; }
			if(positions[i].y < .9f) { velocities[i].y = -velocities[i].y; }
			if(positions[i].y > -.9f) { velocities[i].y = -velocities[i].y; }
			positions[i].x += velocities[i].x * tdelta;
			positions[i].y += velocities[i].y * tdelta;
		}
		
		
		
		glClear( GL_COLOR_BUFFER_BIT );
		for(int i = 0; i < NUM_OF_OBJECTS; i++)
		{
			DrawCircle(positions[i].x, positions[i].y, .1f, 36);
		}
		SDL_GL_SwapWindow( window );
	}

	printf("Average Number of Collisions per Second: %f\n", (float)num_of_collisions/total_time);
	SDL_Quit();

	return 0;

}
