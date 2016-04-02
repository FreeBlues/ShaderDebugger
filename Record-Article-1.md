#	OpenGL ES 2.0 Shader 调试新思路(一): 改变提问方式-是什么(答案是具体值)与是不是(答案是布尔值)

#	目录

##	背景介绍

###	问题描述

`Codea` 是 `iPad` 上的一款很方便的开发软件, 尤其是它支持 `OpenGL ES 2.0/3.0`, 支持着色器 `shader`, 可以直接写代码操纵 `GPU`. 不过也有不太方便的地方, 那就是在 `Codea` 上写 `OpenGL ES 2.0 Shader` 代码的时候发现跟踪 `shader` 内部使用的变量特别困难, 因为 `GPU` 就像一个黑洞, 程序员可以通过程序向 `vertex shader` 和 `fragment shader` 传递数据进去, 但是却没办法把 `shader` 的变量值传回来, 这样就导致在调试 `shader` 代码时看不到内部的变化, 有时候出了问题就得左右推测, 以往 `打印/输出` 变量值的调试方法也失效了, 结果使得调试 `shader` 代码比较困难.

###	已知条件

但是 `shader` 还是要输出信息的, 只不过它输出的信息是 `gl_Position` 和 `gl_FragColor`, 前者是一个四维向量用于设定屏幕图像像素点坐标, 后者也是一个四维向量用于设定颜色值, 而这两个信息是无法直接为我们输出变量值的. 那么是否可以做一点文章, 通过一种间接的方式来传递我们需要知道的信息呢? 

##	Intro

When we program for `OpenGL ES 2.0 Shader` on `Codea`, we find that it is difficult to debug the var used in `shader`. The `GPU` is like a balck hole, programer can transfer data to the `vertex shader` and `fragment shader`, but has no method to fetch the value from `shader`, because of this we can not see the detail inside `shader`. When the `shader` has error, we have to guess and guess. The skill of debug -`print/optput`- can not take effact, so it is difficult to debug the `shader` code.

###	What we have

At last the `shader` will output something, its output is two vars of `gl_Position` and `gl_FragColor`, the first is a vec4 used for setting the coordinate of pixel, the second is also a vec4 used for setting the color of pixel, but the two vars can not output the values of other vars what we want directlly. Then can we get a solution, to get the info undirectlly?

##	解决思路

###	转换思维

昨天晚上, 在调试一段简单但是比较有趣的 `shader` 代码时, 忽然产生了一个灵感:为什么不改变一下对 `shader` 提问的方式? 我们之前在调试普通程序时使用的 `打印/输出` 技巧实际上等价于向计算机提出一个问题: 请告诉我那个变量的值是多少? 很显然, `shader` 程序没有办法直接告诉我们那个变量是多少, 那么换一个思维, 改成问计算机: 请告诉我那个变量的值是不是大于100? 这下就简单了, 因为 `shader` 是很容易回答这个问题的, 因为这个问题的答案要么是 `true`, 要么是 `false`.

###	Change our mind

Yesterday night when I was trying to write some simple but interesting `shader` code, suddenly I have an idea: Why not change the method of question to the `shader`?

###	第一种简单方案

我们很容易设计一段 `shader` 绘图代码, 如果答案是 `true`, 那么我们在指定区域挥着红色, 如果答案是 `false`, 那么我们在指定区域绘制绿色, 这样 `GPU` 就可以通过屏幕间接地把我们需要的信息传递出来了.

###	1st simple solution

It is easy to design the `shader` code which draw red color on specified area when the answer is `true`, and draw green color when the answer is `false`, then the `GPU` can give us the info what we need throughout the screen undirectlly.

假设要观察的变量为 `myVar`, 想要观察它是否大于100, `shader`实现代码如下:

Assume we want to watch the var `myVar`, want to know if it is bigger than 100, the `shader` code is below:

```
//这段代码放在 fragment shader 的 main 函数的最后面
void main()

...

	// 取得坐标 get the coordinate
	float x = vTexCoord.x;
    float y = vTexCoord.y;

    // 设定调试区显示范围为右上角 set the debug area is up/right corner
    if(x > 0.9 && y > 0.9) {
    	if(myVar > 100){
			// 答案为 true 则设置调试区颜色为红色
			gl_FragColor = vec4(1,0,0,.5);
		}else{
			// 答案为 false 则设置调试区颜色为红色
			gl_FragColor = vec4(1,0,0,.5);
		}
	}
}
```

完整的 `shader` 代码为:

The whole `shader` code is below:

```
myShader = {
vsBase = [[
// vertex shader 代码
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
// fragment shader 代码
precision highp float;
uniform lowp sampler2D texture;
varying lowp vec4 vColor;
varying highp vec2 vTexCoord;

void main() {
    lowp vec4 col = texture2D( texture, vTexCoord ) * vColor;

	// 默认全部设置为白色
    gl_FragColor = vec4(1,1,1,1);
    
    // 测试变量 myVar, 可分别设置为 >100 和 <=100 两个区间的值
    int myVar = 1;
    
	// 取得坐标
	float x = vTexCoord.x;
    float y = vTexCoord.y;

    // 设定调试区显示范围为右上角
    if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
    	if(myVar > 100){
			// 答案为 true 则设置调试区颜色为红色
			gl_FragColor = vec4(1,0,0,1);
		}else {
			// 答案为 false 则设置调试区颜色为红色
			gl_FragColor = vec4(0,1,0,1);
		}
	}
}
]]
}
```
配套的 `Codea` 代码为:

The `Codea` code is:

```
-- Shader debug
displayMode(OVERLAY)
function setup()
    m = mesh()

    -- 首先得有顶点，后续要把顶点数据传给 vertex Shader 处理程序
    m:addRect(WIDTH/2,HEIGHT/2,WIDTH,HEIGHT)    

    -- 设置 shader
    m.shader = shader(myShader.vsBase,myShader.fsBase)
    
    -- 要有 texture 才能设置颜色  
    -- m.texture = "Documents:univer"
    m:setColors(color(220,200,200,255))
    
    -- 观察
    parameter.watch("m.shader.modelViewProjection")
    parameter.watch("m.shader.uResolution")
    parameter.watch("m.vertices[1]")
    
    -- 显示 FPS 和内存使用情况
    elapsedTimeMem = ElapsedTime
    FPS = 0
    elapsedTimeFPS = ElapsedTime
    memory = 0
    msg = ""
end

function draw()
    background(0)
    m:draw()

    
    ortho()
    viewMatrix(matrix())
    -- display some metrics (FPS, memory)
    metrics()
end

function touched(touch)
    
end

```



没有`Codea`的用户可以在`XCode`下编译该项目, 然后在模拟器查看执行结果, `XCode` 项目文件下载地址:


运行截图如下:
截图1

###	可以显示变量值的方案

非常好, 通过上面的试验, 我们终于可以大致了解 `shader` 中变量的情况了, 比如说它是不是大于某个数, 是不是小于某个数, 是不是正数, 是不是负数, 等等. 但是这种调试信息还是太粗糙, 而且用起来也比较繁琐. 那么更进一步, 我们还是希望能看到变量的具体的值, 前面说过 `shader` 没办法像 `printf` 一样, 直接把变量值打印到屏幕, 但是我们知道我们实际上可以通过 `shader` 完全控制屏幕输出, 所以理论上我们可以在屏幕上绘制出任何内容, 包括数字.

Very well, with the experiment above, we can know the brief of the var in `shader`. But the info is too simple, and it is complicated to use. We want to watch the exact value, we know we can control the whole screen with `shader`, so in abstracto we can draw anything to screen include number.

现在先简化一下问题, 假设 `myVar` 的取值范围是整数 `0~9`, 那么我们可以设计一种对应算法, 处理逻辑是这样的:

Now let us make the problem simple, assume `myVar` is a int and its region is [0,9], then we can get a logic: 

```
如果 myVar 是 1, 那么我们在指定的区域内把指定的像素点用指定的颜色绘制(绘制出1);		
如果 myVar 是 2, 那么我们在指定的区域内把指定的像素点用指定的颜色绘制(绘制出2);		
...		
如果 myVar 是 9, 那么我们在指定的区域内把指定的像素点用指定的颜色绘制(绘制出9);		
如果 myVar 是 0, 那么我们在指定的区域内把指定的像素点用指定的颜色绘制(绘制出0);		
```

```
if myVar is 1, then draw specified pixels in a specified area with specified color(draw 1);
if myVar is 2, then draw specified pixels in a specified area with specified color(draw 2);
...
if myVar is 9, then draw specified pixels in a specified area with specified color(draw 9);
if myVar is 0, then draw specified pixels in a specified area with specified color(draw 0);
```

听起来不错, 这样我们就可以让 `shader` 输出 `1~0` 10个数字了. 继续简化问题, 先从最简单的地方入手, 我们试着处理一下 `1`, 暂时不管 `myVar` 的值, 我们只是简单地在屏幕上绘制一个 `1`, 那么代码如下:

It looks good, now we can make `shader` output the 10 number of `1~0`.  Continue to make the problem simpler, start with the easiest place, only to draw a number `1` on the screen, the code is below:

```
// 取得坐标
float x = vTexCoord.x;
float y = vTexCoord.y;

// 先设置好调试区的范围
if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
	// 设置正方形区域的颜色为白色作为背景色
	gl_FragColor = vec4(1,1,1,1);
	// 相当于在一个正方形内绘制一个 `1`, 我们选择最右边
	if( x > 0.99 ){
		// 最右边设置为绿色
		gl_FragColor = vec4(0,1,0,1);
	}
}
```
截图如下:

很好, 继续, 增加一个判断条件 `myVar是否为1`, 否则只要执行到这个区域坐标一律会绘制一个白底绿色的数字`1`, 如下:

Ok, it works, now we can add more, for example we can add a judgement with `if myVar is number 1`, if it is `true` the `shader` will draw a green number 1 with white background in this area.

```
// 取得坐标
float x = vTexCoord.x;
float y = vTexCoord.y;

// 先设置好调试区的范围
if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
	// 设置正方形区域的颜色为白色作为背景色
	gl_FragColor = vec4(1,1,1,1);
	// 相当于在一个正方形内绘制一个 `1`, 我们选择最右边
	if( myVar == 1 && x > 0.99 ){
		// 最右边设置为绿色
		gl_FragColor = vec4(0,1,0,1);
	}
}
```

接着我们可以把 `2~0` 的数字全部以这种方式绘制出来, 为了简单起见, 数字造型全部采用类似7段数码管的那种风格, 伪码如下:

One by one, we can draw the number `2~0` with the same method:

```
// 取得坐标
float x = vTexCoord.x;
float y = vTexCoord.y;

// 先设置好调试区的范围
if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
	// 设置正方形区域的颜色为白色作为背景色
	gl_FragColor = vec4(1,1,1,1);
	// 相当于在一个正方形内绘制一个 `1`, 我们选择最右边
	if( myVar == 1 && x > 0.99 ){
		// 最右边设置为绿色
		gl_FragColor = vec4(0,1,0,1);
	}
	if( myVar == 2 && (2的绘制坐标范围) ){
		// 最右边设置为绿色
		gl_FragColor = vec4(0,1,0,1);
	}
	...
	if( myVar == 0 && (0的坐标绘制范围) ){
		// 最右边设置为绿色
		gl_FragColor = vec4(0,1,0,1);
	}
}
```

回头看看代码, 发现貌似有很多重复的地方, 稍微合并一下, 伪码如下:

Optimize the code:

```
// 取得坐标
float x = vTexCoord.x;
float y = vTexCoord.y;

// 先设置好调试区的范围
if(x > 0.9 && x < 1.0 && y > 0.9 && y < 1.0) {
	// 设置正方形区域的颜色为白色作为背景色
	gl_FragColor = vec4(1,1,1,1);
	// 相当于在一个正方形内绘制一个 `1`, 我们选择最右边
	if(( myVar == 1 && x > 0.99 ) ||
	   ( myVar == 2 && (2的绘制坐标范围)) ||
	   ...
	   ( myVar == 0 && (0的坐标绘制范围))
	  )
	{
		// 最右边设置为绿色
		gl_FragColor = vec4(0,1,0,1);
	}	
}
```

基本上就是这样样子, 把它写成函数形式, 代码如下:

It is ok, change it to a function:

```
// 构造数字
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
    
    // 设定调试区显示范围
    if(x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        // 设置调试区背景色的半透明的蓝色
        gl_FragColor = vec4(0,0,1,.5);
        // 分别绘制出 LED 形式的数字 1~0 
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
        	// 设置数字颜色为绿色
            gl_FragColor = vec4(0,1,0,1);
        }
    }
}
```

完整的 `shader` 代码如下:

The whole `shader` code is below:

```
myShader = {
vsBase = [[
// vertex shader 代码
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
// fragment shader 代码
precision highp float;
uniform lowp sampler2D texture;
varying lowp vec4 vColor;
varying highp vec2 vTexCoord;

void ledChar(int,float,float,float,float);

void main() {
    lowp vec4 col = texture2D( texture, vTexCoord ) * vColor;

	// 默认全部设置为黑色
    gl_FragColor = vec4(.1,.1,.1,1);
    
    // 在右上角显示1
    ledChar(1, 0.9, 0.1, 0.9, 0.1);
}

// 构造数字
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
    
    // 设定调试区显示范围
    if(x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        // 设置调试区背景色为半透明的蓝色
        gl_FragColor = vec4(0.2,0.8,0.2,.5);
        // 分别绘制出 LED 形式的数字 1~0 
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
        	// 设置数字颜色为绿色
            gl_FragColor = vec4(0,1,0,1);
        }
    }
}
]]
}
```

运行截图如下:
截图2

补充, 代码完成后, 忽然想到另一种实现 `LED` 字型的算法, 我们上面的算法是计算7段数码管每段的坐标范围, 然后绘制, 最多需要绘制5段(2,5,6,8,9), 判断语句写起来比较长, 其实我们可以反其道而行之, 意思就是说不画数字笔画, 改画数字笔画间的矩形(每个数字都可以用1个或2个小矩形分割出来), 上个示意图:


这样考虑角度一变, 就发现其实用矩形在这种场景下更简单, 代码如下:



理论上来说, 有了上面这些基础, 我们就可以自由地通过 `shader` 输出要观察变量的值了, 不过貌似有一个 `bug`: 重复运行 `ledChar` 函数会导致花屏, 暂时还没搞清楚问题原因.

在显示数字的方向上, 今天还想到几种获得数字字型的办法, 一种是通过多个 `vec4` 或 `mat4` 来传, 另一种是通过 `texture` 来传, 大致考虑了下, 感觉还是通过 `texture` 传比较简单, 因为它是直接传图, 不需要再自己算像素点坐标了, 这个想法也准备后续试验一下.

另外, 今天在有了这个思路之后, 去搜索了一下 `shader debug`, 结果在 `StackOverflow` 网站发现也有人问同样的问题, 然后有人给出了本文提到的第一种简单方法, 还有人提出一种很有想象力的方案, 那就是把要观察的数据可视化, 直接用图像来表达数字, 看着挺有趣, [点击网页链接](http://stackoverflow.com/questions/2508818/how-to-debug-a-glsl-shader), 准备后面也试试. 

In abstracto, with the foundation code above, we can watch the var value in `shader` easily, but unfortunately there is a big `bug` I have not found: when in one mesh run the function `ledChar` twice, it will make the screen messy.

About how to display number font with `shader`, I have thought of some other method, one is using `vec4` or `mat4` to transfer the font data , another is using the `texture`. I think the last one will be esaier. I will try these ideas later.

Btw, when I have had these ideas, I thought maybe others will have the same idea, so I searched for the keywords `shader debug`, then I found in `StackOverflow` some programers were talking about it, one person give the answer similar with the first solution of mine, and one person gave a solution full of imagination--visualization. It looks very interesting. [The article linke](http://stackoverflow.com/questions/2508818/how-to-debug-a-glsl-shader), I will try the visualization later. 
