// by Gefa, gefasio@gmail.com

#include <oslib/oslib.h>
#include <chipmunk/chipmunk.h>

PSP_MODULE_INFO("chipmunktest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(12*1024);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init OSLib:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int initOSLib(){
    oslInit(0);
    oslInitGfx(OSL_PF_8888, 1);
    oslInitAudio();
    oslSetQuitOnLoadFailure(1);
    oslSetKeyAutorepeatInit(40);
    oslSetKeyAutorepeatInterval(10);
    return 0;
}

void drawCircle(cpFloat x, cpFloat y, cpFloat r, cpFloat a)
{
    const int segs = 15;
	const cpFloat coef = 2.0*M_PI/(cpFloat)segs;
	
	float xVerts[16];
	float yVerts[16];
	
    int n;
	for(n = 0; n <= segs; n++){
	   cpFloat rads = n*coef;
	   xVerts[segs-n] = r*cos(rads + a) + x;
	   yVerts[segs-n] = r*sin(rads + a) + y;
	}
	
    for(n=0; n<segs; n++){
         if (n != segs-1)
             oslDrawLine((int)xVerts[n], 272-(int)yVerts[n], (int)xVerts[n+1], 272-(int)yVerts[n+1], RGBA(0,0,0,255));
         else
             oslDrawLine((int)xVerts[n], 272-(int)yVerts[n], (int)xVerts[0], 272-(int)yVerts[0], RGBA(0,0,0,255));      
     }
}

void drawCircleShape(cpShape *shape)
{
	cpBody *body = shape->body;
	cpCircleShape *circle = (cpCircleShape *)shape;
	cpVect c = cpvadd(body->p, cpvrotate(circle->c, body->rot));
	drawCircle(c.x, c.y, circle->r, body->a);
}


void drawSegmentShape(cpShape *shape)
{
	cpBody *body = shape->body;
	cpSegmentShape *seg = (cpSegmentShape *)shape;
	cpVect a = cpvadd(body->p, cpvrotate(seg->a, body->rot));
	cpVect b = cpvadd(body->p, cpvrotate(seg->b, body->rot));

	oslDrawLine((int)a.x, 272-(int)a.y, (int)b.x, 272-(int)b.y, RGBA(0,0,0,255));
}

void drawPolyShape(cpShape *shape)
{
     float xVerts[255];
	 float yVerts[255];
	 
	 cpBody *body = shape->body;
	 cpPolyShape *poly = (cpPolyShape *)shape;
	 cpVect v;
	 
	 int num = poly->numVerts;
	 cpVect *verts = poly->verts;
	 
	 int i;
	 for(i=0; i<num; i++){
		 v = cpvadd(body->p, cpvrotate(verts[i], body->rot));
		 xVerts[i] = v.x;
		 yVerts[i] = v.y;
	 }
	
	 for(i=0; i<num; i++){
         if (i != num-1)
             oslDrawLine((int)xVerts[i], 272-(int)yVerts[i], (int)xVerts[i+1], 272-(int)yVerts[i+1], RGBA(0,0,0,255));
         else
             oslDrawLine((int)xVerts[i], 272-(int)yVerts[i], (int)xVerts[0], 272-(int)yVerts[0], RGBA(0,0,0,255));      
     }
}

void drawObject(void *ptr, void *unused)
{
	cpShape *shape = (cpShape *)ptr;
	switch(shape->klass->type){
		case CP_CIRCLE_SHAPE:
            drawCircleShape(shape);
			break;
		case CP_SEGMENT_SHAPE:
			drawSegmentShape(shape);
			break;
		case CP_POLY_SHAPE:
			drawPolyShape(shape);
			break;
		default:
			printf("Bad enumeration in drawObject().\n");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){
    int skip = 0;
    
    cpVect mousePos = cpv(240,136);;

    initOSLib();
    cpInitChipmunk();
    
    cpSpace *space;
    cpBody *body, *staticBody;
	cpShape *shape;
	cpMouse *mouse;
	
    space = cpSpaceNew();
	cpSpaceResizeStaticHash(space, 20.0, 999);
	space->gravity = cpv(0, -100);
	
	mouse = cpMouseNew(space);
	
    shape = cpSegmentShapeNew(&space->staticBody, cpv(0,0), cpv(480,0), 0.0f);
	shape->e = 1.0; shape->u = 1.0;
	cpSpaceAddStaticShape(space, shape);
	
	shape = cpSegmentShapeNew(&space->staticBody, cpv(0,0), cpv(0,272), 0.0f);
	shape->e = 1.0; shape->u = 1.0;
	cpSpaceAddStaticShape(space, shape);
	
	shape = cpSegmentShapeNew(&space->staticBody, cpv(480,0), cpv(480,272), 0.0f);
	shape->e = 1.0; shape->u = 1.0;
	cpSpaceAddStaticShape(space, shape);
	
	int num = 4;
	cpVect verts[] = {
		cpv(-10,-10),
		cpv(-10, 10),
		cpv( 10, 10),
		cpv( 10,-10),
	};
			
	int i,j;
	for(i=0; i<14; i++){
		for(j=0; j<=i; j++){
			body = cpBodyNew(1.0, cpMomentForPoly(1.0, num, verts, cpvzero));
			body->p = cpv(240 + j*20 - i*10, 300 - i*20);
			cpSpaceAddBody(space, body);
			shape = cpPolyShapeNew(body, num, verts, cpvzero);
			shape->e = 0.0; shape->u = 0.8;
			cpSpaceAddShape(space, shape);
		}
	}

    cpFloat radius = 15.0;
	body = cpBodyNew(10.0, cpMomentForCircle(10.0, 0.0, radius, cpvzero));
	body->p = cpv(240, radius);
	cpSpaceAddBody(space, body);
	shape = cpCircleShapeNew(body, radius, cpvzero);
	shape->e = 0.0; shape->u = 0.9;
	cpSpaceAddShape(space, shape);

    while(!osl_quit){
        if (!skip){
	        cpFloat dt = 1.0/60.0;
		    cpSpaceStep(space, dt);
                   
            oslStartDrawing();

            oslClearScreen(RGBA(255,255,255,255));
            
            oslDrawString(10, 10, "Chipmunk 5.3.1 with OSL - demo -");
     
            cpSpaceHashEach(space->activeShapes, &drawObject, NULL);
            cpSpaceHashEach(space->staticShapes, &drawObject, NULL);
            
            //Draw cursor
            oslDrawLine(mousePos.x, mousePos.y-3, mousePos.x, mousePos.y+3, RGBA(100,0,0,255));
            oslDrawLine(mousePos.x-3, mousePos.y, mousePos.x+3, mousePos.y, RGBA(100,0,0,255));

            oslEndDrawing();
            
            oslReadKeys();
            if (osl_keys->held.left)
               mousePos.x -= 3;
            else if (osl_keys->held.right)
               mousePos.x += 3;
            else if (osl_keys->held.up)
               mousePos.y -= 3;
            else if (osl_keys->held.down)
               mousePos.y += 3;
            
            cpMouseMove(mouse, cpv(mousePos.x, 272 - mousePos.y));
            
            if (osl_keys->held.cross)
            {		
                 shape = cpSpacePointQueryFirst(space, cpv(mousePos.x, 272 - mousePos.y), GRABABLE_MASK_BIT, 0);
                 if (shape){
                     mousePos = cpv(shape->body->p.x, 272-shape->body->p.y);
                 }
                 cpMouseGrab(mouse, cpv(mousePos.x, 272 - mousePos.y), 0, 50000);
            }
            else if (osl_keys->released.cross)
            {		
                 cpMouseRelease(mouse);
            }
        }
        oslEndFrame();
        skip = oslSyncFrame();
    }
    
    cpMouseFree(mouse);
    cpSpaceFreeChildren(space);
	cpSpaceFree(space);
    
    //Quit OSL:
    oslEndGfx();

    sceKernelExitGame();
    return 0;

}
