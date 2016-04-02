#      How to watch a var in shader? -A new solution: change the question from What is to Is it

##	Intro

### Description

When we program for `OpenGL ES 2.0 Shader` on `Codea`, we find that it is difficult to debug the var used in `shader`. The `GPU` is like a balck hole, programer can transfer data to the `vertex shader` and `fragment shader`, but has no method to fetch the value from `shader`, because of this we can not see the detail inside `shader`. When the `shader` has error, we have to guess and guess. The skill of debug -`print/optput`- can not take effact, so it is difficult to debug the `shader` code.

###	What we have

At last the `shader` will output something, its output is two vars of `gl_Position` and `gl_FragColor`, the first is a vec4 used for setting the coordinate of pixel, the second is also a vec4 used for setting the color of pixel, but the two vars can not output the values of other vars what we want directlly. Then can we get a solution, to get the info undirectlly?

###	Change our mind

Yesterday night when I was trying to write some simple but interesting `shader` code, suddenly I have an idea: Why not change the method of question to the `shader`?

###	The 1st simple solution

It is easy to design the `shader` code which draw red color on specified area when the answer is `true`, and draw green color when the answer is `false`, then the `GPU` can give us the info what we need throughout the screen undirectlly.

Assume we want to watch the var `myVar`, want to know if it is bigger than 100, the `shader` code is below:

```
void main()

...


	float x = vTexCoord.x;
    float y = vTexCoord.y;

    //  set the debug area is up/right corner
    if(x > 0.9 && y > 0.9) {
    	if(myVar > 100){
			// true is red
			gl_FragColor = vec4(1,0,0,.5);
		}else{
			// false is green
			gl_FragColor = vec4(0,1,0,.5);
		}
	}
}
```

The whole `shader` code is below:

```
myShader = {
vsBase = [[
// vertex shader 
uniform mat4 modelViewProjection;
uniform vec2 uResolution;

attribute vec4 position; 
attribute vec4 color; 
attribute vec2 texCoord;

varying lowp vec4 vColor; 
varying highp vec2 vTexCoord;

void main() {
	vColor=color;
	vTexCoord = texCoord;

	gl_Position = modelViewProjection * position;
}
]],
fsBase = [[
// fragment shader 
precision highp float;
uniform lowp sampler2D texture;
varying lowp vec4 vColor;
varying highp vec2 vTexCoord;

void main() {
    lowp vec4 col = texture2D( texture, vTexCoord ) * vColor;

	// default is white
    gl_FragColor = vec4(1,1,1,1);
    
    // test var 
    int myVar = 1;
    
	float x = vTexCoord.x;
    float y = vTexCoord.y;

    // debug area
    if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
    	if(myVar > 100){
			// true is red
			gl_FragColor = vec4(1,0,0,1);
		}else {
			// false is green
			gl_FragColor = vec4(0,1,0,1);
		}
	}
}
]]
}
```

The `Codea` code is:

```
-- Shader debug
displayMode(OVERLAY)
function setup()
    m = mesh()
    m:addRect(WIDTH/2,HEIGHT/2,WIDTH,HEIGHT)    
    m.shader = shader(myShader.vsBase,myShader.fsBase)
    
    -- m.texture = "Documents:univer"
    m:setColors(color(220,200,200,255))
    
    parameter.watch("m.shader.modelViewProjection")
    parameter.watch("m.shader.uResolution")
    parameter.watch("m.vertices[1]")
    
end

function draw()
    background(0)
    m:draw()   
end

function touched(touch)
    
end

```

### The 2nd solution: Show the value

Very well, with the experiment above, we can know the brief of the var in `shader`. But the info is too simple, and it is complicated to use. We want to watch the exact value, we know we can control the whole screen with `shader`, so in abstracto we can draw anything to screen include number.


Now let us make the problem simple, assume `myVar` is a int and its region is [0,9], then we can get a logic: 


```
if myVar is 1, then draw specified pixels in a specified area with specified color(draw 1);
if myVar is 2, then draw specified pixels in a specified area with specified color(draw 2);
...
if myVar is 9, then draw specified pixels in a specified area with specified color(draw 9);
if myVar is 0, then draw specified pixels in a specified area with specified color(draw 0);
```


It looks good, now we can make `shader` output the 10 number of `1~0`.  Continue to make the problem simpler, start with the easiest place, only to draw a number `1` on the screen, the code is below:

```
float x = vTexCoord.x;
float y = vTexCoord.y;

// debug area
if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
	// background is white
	gl_FragColor = vec4(1,1,1,1);
	// draw 1 in a rect
	if( x > 0.99 ){
		// the right set green
		gl_FragColor = vec4(0,1,0,1);
	}
}
```


Ok, it works, now we can add more, for example we can add a judgement with `if myVar is number 1`, if it is `true` the `shader` will draw a green number 1 with white background in this area.

```
float x = vTexCoord.x;
float y = vTexCoord.y;

// debug area
if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
	// background is white
	gl_FragColor = vec4(1,1,1,1);
	// draw 1 in a rect
	if( myVar == 1 && x > 0.99 ){
		// set green
		gl_FragColor = vec4(0,1,0,1);
	}
}
```

One by one, we can draw the number `2~0` with the same method:

```
float x = vTexCoord.x;
float y = vTexCoord.y;

// debug area
if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
	//  background is white
	gl_FragColor = vec4(1,1,1,1);
	// draw 1 in a rect
	if( myVar == 1 && x > 0.99 ){
		// set green
		gl_FragColor = vec4(0,1,0,1);
	}
	if( myVar == 2 && (the coordinate of 2) ){
		// set green
		gl_FragColor = vec4(0,1,0,1);
	}
	...
	if( myVar == 0 && (the coordinate of 0) ){
		// set green
		gl_FragColor = vec4(0,1,0,1);
	}
}
```


Optimize the code:

```
float x = vTexCoord.x;
float y = vTexCoord.y;

// debug area
if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
	// background is white
	gl_FragColor = vec4(1,1,1,1);
	// draw 1 in a rect
	if(( myVar == 1 && x > 0.99 ) ||
	   ( myVar == 2 && (the coordinate of 2)) ||
	   ...
	   ( myVar == 0 && (the coordinate of 0))
	  )
	{
		// set green
		gl_FragColor = vec4(0,1,0,1);
	}	
}
```


It is ok, change it to a function:

```
// LED char
void ledChar(int n, float xa,float xb, float ya, float yb){
    float x = vTexCoord.x;
    float y = vTexCoord.y;
    float x1 = xa; 
    float x2 = xa+xb;
    float y1 = ya;
    float y2 = ya+yb;
    float ox = (x2+x1)/2.0;
    float oy = (y2+y1)/2.0;
    float dx = (x2-x1)/10.0;
    float dy = (y2-y1)/10.0;
    float b = (x2-x1)/20.0;
    int num = n;
    
    // debug area
    if(x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        // background is white
        gl_FragColor = vec4(0,0,1,.5);
        // draw 1~0 in a rect
        if((num==1 && (x > x2-dx)) ||
           (num==2 && ((y > y2-dy) || (x > x2-dx && y > oy-dy/2.0) || (y > oy-dy/2.0 && y < oy+dy/2.0) || (x < x1+dx && y < oy+dy/2.0) || (y < y1+dy))) ||
           (num==3 && ((y > y2-dy) || (x > x2-dx) || (y > oy-dy/2.0 && y < oy+dy/2.0) ||  (y < y1+dy))) ||
           (num==4 && ((x < x1+dx && y > oy-dy/2.0) ||(x > x2-dx) || (y > oy-dy/2.0 && y < oy+dy/2.0))) ||
           (num==5 && ((y > y2-dy) || (x < x1+dx && y > oy-dy/2.0)|| (y > oy-dy/2.0 && y < oy+dy/2.0) || (x>x2-dx && y <oy-dy/2.0) || (y<y1+dy))) ||
           (num==6 && ((y > y2-dy) || (x < x1+dx)|| (y > oy-dy/2.0 && y < oy+dy/2.0) || (x>x2-dx && y <oy-dy/2.0) || (y<y1+dy))) ||
           (num==7 && ((y > y2-dy) || (x > x2-dx))) ||
           (num==8 && ((y > y2-dy) || (x < x1+dx)|| (y > oy-dy/2.0 && y < oy+dy/2.0) || (x>x2-dx) || (y<y1+dy))) ||
           (num==9 && ((y > y2-dy) || (x < x1+dx && y > oy-dy/2.0)||(y > oy-dy/2.0 && y < oy+dy/2.0)|| (x>x2-dx) || (y<y1+dy))) ||
           (num==0 && ((y > y2-dy) || (x < x1+dx) || (x>x2-dx) || (y<y1+dy)))
           )
        {
        	// set green
            gl_FragColor = vec4(0,1,0,1);
        }
    }
}
```

The whole `shader` code is below:

```
myShader = {
vsBase = [[
// vertex shader 
uniform mat4 modelViewProjection;
uniform vec2 uResolution;

attribute vec4 position; 
attribute vec4 color; 
attribute vec2 texCoord;

varying lowp vec4 vColor; 
varying highp vec2 vTexCoord;

void main() {
	vColor=color;
	vTexCoord = texCoord;

	gl_Position = modelViewProjection * position;
}
]],
fsBase = [[
// fragment shader 
precision highp float;
uniform lowp sampler2D texture;
varying lowp vec4 vColor;
varying highp vec2 vTexCoord;

void ledChar(int,float,float,float,float);

void main() {
    lowp vec4 col = texture2D( texture, vTexCoord ) * vColor;

	// default is white
    gl_FragColor = vec4(.1,.1,.1,1);
    
    // show 1
    ledChar(1, 0.9, 0.1, 0.9, 0.1);
}

// LED char
void ledChar(int n, float xa,float xb, float ya, float yb){
    float x = vTexCoord.x;
    float y = vTexCoord.y;
    float x1 = xa; 
    float x2 = xa+xb;
    float y1 = ya;
    float y2 = ya+yb;
    float ox = (x2+x1)/2.0;
    float oy = (y2+y1)/2.0;
    float dx = (x2-x1)/10.0;
    float dy = (y2-y1)/10.0;
    float b = (x2-x1)/20.0;
    int num = n;
    
    // debug area
    if(x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        // set blue
        gl_FragColor = vec4(0.2,0.8,0.2,.5);
        // draw number 1~0 
        if((num==1 && (x > x2-dx)) ||
           (num==2 && ((y > y2-dy) || (x > x2-dx && y > oy-dy/2.0) || (y > oy-dy/2.0 && y < oy+dy/2.0) || (x < x1+dx && y < oy+dy/2.0) || (y < y1+dy))) ||
           (num==3 && ((y > y2-dy) || (x > x2-dx) || (y > oy-dy/2.0 && y < oy+dy/2.0) ||  (y < y1+dy))) ||
           (num==4 && ((x < x1+dx && y > oy-dy/2.0) ||(x > x2-dx) || (y > oy-dy/2.0 && y < oy+dy/2.0))) ||
           (num==5 && ((y > y2-dy) || (x < x1+dx && y > oy-dy/2.0)|| (y > oy-dy/2.0 && y < oy+dy/2.0) || (x>x2-dx && y <oy-dy/2.0) || (y<y1+dy))) ||
           (num==6 && ((y > y2-dy) || (x < x1+dx)|| (y > oy-dy/2.0 && y < oy+dy/2.0) || (x>x2-dx && y <oy-dy/2.0) || (y<y1+dy))) ||
           (num==7 && ((y > y2-dy) || (x > x2-dx))) ||
           (num==8 && ((y > y2-dy) || (x < x1+dx)|| (y > oy-dy/2.0 && y < oy+dy/2.0) || (x>x2-dx) || (y<y1+dy))) ||
           (num==9 && ((y > y2-dy) || (x < x1+dx && y > oy-dy/2.0)||(y > oy-dy/2.0 && y < oy+dy/2.0)|| (x>x2-dx) || (y<y1+dy))) ||
           (num==0 && ((y > y2-dy) || (x < x1+dx) || (x>x2-dx) || (y<y1+dy)))
           )
        {
        	// set green
            gl_FragColor = vec4(0,1,0,1);
        }
    }
}
]]
}
```

In abstracto, with the foundation code above, we can watch the var value in `shader` easily, but unfortunately there is a big `bug` I have not found: when in one mesh run the function `ledChar` twice, it will make the screen messy.

About how to display number font with `shader`, I have thought of some other method, one is using `vec4` or `mat4` to transfer the font data , another is using the `texture`. I think the last one will be esaier. I will try these ideas later.

Btw, when I have had these ideas, I thought maybe others will have the same idea, so I searched for the keywords `shader debug`, then I found in `StackOverflow` some programers were talking about it, one person give the answer similar with the first solution of mine, and one person gave a solution full of imagination--visualization. It looks very interesting. [The article linke](http://stackoverflow.com/questions/2508818/how-to-debug-a-glsl-shader), I will try the visualization later. 
