
// Provisional API for Display (List) Processor

Animation ButterflyAnim = AnimLoadFromFile("butterfly.anim");

// An actor is a visually distinct animated element with its own behavior (e.g butterfly)
typedef struct {
    int     x;                  // x, y are the origin of the coord sys within which the animation is located 
    int     y;                  // x, y are defined in screen coords
    Animation *animPtr;         
    UpdateFn *updateFn;         // pointer to a function (state machine) that defines the interactive behavior of actor
} Actor;

// Animation: a collection of static vector drawings (Frames), including timing information
typedef struct {
    int numFrame;
    Frame[] frames;
} Animation;

// Frame is a collection of paint-to-point commands necessary to draw a static image
typedef struct {
    int time;           // time in milliseconds since the start of the animation at which point the frame should be shown
    int numPoints;
    DPPaintToPoint[] points;
} Frame;

typedef struct {
    unsigned char r, g, b;
} DPColor; 

typedef struct {
    unsigned int x, y;
    DPColor color;
} DPPaintToPoint; // Need to come up with a good name for this 

// Clear the display list
void DPClear();

// Output the "rendered" display list to the Point Player
void DPDraw();

// Add the line to the display list
// Return true if line was added to the display list, false otherwise
// USed by the animaton engine to describe a frame
bool DPAddPoint(DPPoint point);


/* Sample usage:

    void() MainGameLoop() {
        last = 0;
        
        while (true) {
            // units are usec
            while ((now() - last) < 1000); // will block until at least 1ms has gone by 
            last = now;
            
            readInputs();
            DPClear(); 
            for (i = 0; i<numActors; i++) {
                actors[i].updateFn();
            }
            DPDraw();
        }
        
    }
    
    void ActorUpdate(Actor *actorPtr)
    { 
        ...
        
        AnimUpdate(actorPtr->animPtr, actorPtr->x, actorPtr->y);            
        
    }


    void AnimUpdate(Animation *animPtr)
    {
        // check to see if t millis have elsapsed since begining on animation, if so update frame index or tell me what the frame should be
        int timeOffset = currentTime() - ThisAnimStartTime;
        
        int frameI = FrameAtTime(animPtr,timeOffset);
        Frame thisFrame = animPtr->Frame[frameI];   
                
        for (int ptI = 0; ptI < thisFrame.numPoints; ptI++) {
            DPAddPoint(thisFrame.points[ptI]);
        }
    }
    

    
*/


/* example animation - frame 0: X, frame 1:  +
Frame0.time = 0
Frame0.numPoints = 4 
Frame0 Point 0 = {x,y = 0,0, color = 0}
Frame0 Point 1 = {x,y = max,max, color = 1}
Frame0 Point 2 = {x,y = 0,max, color = 0}
Frame0 Point 3 = {x,y = max,0, color = 1}

Frame0.time = 500 ms
Frame0.numPoints = 4 
Frame0 Point 0 = {x,y = 0, 1/2, color = 0}
Frame0 Point 1 = {x,y = max,1/2, color = 1}
Frame0 Point 2 = {x,y = 1/2,max, color = 0}
Frame0 Point 3 = {x,y = 1/2,0, color = 1}
*/



/* PROJECT CODING STANDARDS
Fns start with Capital and use Camel - MyFunction
local variable begin with lower case - localVariable
Global variables begin with an upper - GlobalVariable

Function names = NounVerb (eg AnimationUpdate)
Pointers for eg local variable would be - localVariablePtr

*/
