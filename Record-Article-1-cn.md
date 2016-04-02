#	OpenGL ES 2.0 Shader 调试新思路(一): 改变提问方式

## --是什么(答案是具体值) VS 是不是(答案是布尔值)

#	目录

##	背景介绍

###	问题描述

`Codea` 是 `iPad` 上的一款很方便的开发软件, 尤其是它支持 `OpenGL ES 2.0/3.0`, 支持着色器 `shader`, 可以直接写代码操纵 `GPU`. 不过也有不太方便的地方, 那就是在 `Codea` 上写 `OpenGL ES 2.0 Shader` 代码的时候发现跟踪 `shader` 内部使用的变量特别困难, 因为 `GPU` 就像一个黑洞, 程序员可以通过程序向 `vertex shader` 和 `fragment shader` 传递数据进去, 但是却没办法把 `shader` 的变量值传回来, 这样就导致在调试 `shader` 代码时看不到内部的变化, 有时候出了问题就得左右推测, 以往 `打印/输出` 变量值的调试方法也失效了, 结果使得调试 `shader` 代码比较困难.

###	已知条件

但是 `shader` 还是要输出信息的, 只不过它输出的信息是 `gl_Position` 和 `gl_FragColor`, 前者是一个四维向量用于设定屏幕图像像素点坐标, 后者也是一个四维向量用于设定颜色值, 而这两个信息是无法直接为我们输出变量值的. 那么是否可以做一点文章, 通过一种间接的方式来传递我们需要知道的信息呢? 


##	解决思路

###	转换思维

昨天晚上, 在调试一段简单但是比较有趣的 `shader` 代码时, 忽然产生了一个灵感:为什么不改变一下对 `shader` 提问的方式? 我们之前在调试普通程序时使用的 `打印/输出` 技巧实际上等价于向计算机提出一个问题: 请告诉我那个变量的值是多少? 很显然, `shader` 程序没有办法直接告诉我们那个变量是多少, 那么换一个思维, 改成问计算机: 请告诉我那个变量的值是不是大于100? 这下就简单了, 因为 `shader` 是很容易回答这个问题的, 因为这个问题的答案要么是 `true`, 要么是 `false`.


###	第一种简单方案

我们很容易设计一段 `shader` 绘图代码, 如果答案是 `true`, 那么我们在指定区域挥着红色, 如果答案是 `false`, 那么我们在指定区域绘制绿色, 这样 `GPU` 就可以通过屏幕间接地把我们需要的信息传递出来了.


假设要观察的变量为 `myVar`, 想要观察它是否大于100, `shader`实现代码如下:


```
//这段代码放在 fragment shader 的 main 函数的最后面
void main()

...

	// 取得坐标
	float x = vTexCoord.x;
    float y = vTexCoord.y;

    // 设定调试区显示范围为右上角
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
    
end

function draw()
    background(0)
    m:draw()
end

function touched(touch)
    
end

```


没有`Codea`的用户可以在`XCode`下编译该项目, 然后在模拟器查看执行结果, `XCode` 项目文件下载地址[XCode项目文件链接](http://pan.baidu.com/s/1miISuVI):


运行截图如下:

`myVar` 大于 100
![红色](https://static.oschina.net/uploads/img/201603/31014036_Tzbf.png "在这里输入图片标题")

`myVar` 小于等于 100
![绿色](https://static.oschina.net/uploads/img/201603/31014119_ZqAW.png "在这里输入图片标题")


###	可以显示变量值的方案

非常好, 通过上面的试验, 我们终于可以大致了解 `shader` 中变量的情况了, 比如说它是不是大于某个数, 是不是小于某个数, 是不是正数, 是不是负数, 等等. 但是这种调试信息还是太粗糙, 而且用起来也比较繁琐. 那么更进一步, 我们还是希望能看到变量的具体的值, 前面说过 `shader` 没办法像 `printf` 一样, 直接把变量值打印到屏幕, 但是我们知道我们实际上可以通过 `shader` 完全控制屏幕输出, 所以理论上我们可以在屏幕上绘制出任何内容, 包括数字.


现在先简化一下问题, 假设 `myVar` 的取值范围是整数 `0~9`, 那么我们可以设计一种对应算法, 处理逻辑是这样的:

```
如果 myVar 是 1, 那么我们在指定的区域内把指定的像素点用指定的颜色绘制(绘制出1);		
如果 myVar 是 2, 那么我们在指定的区域内把指定的像素点用指定的颜色绘制(绘制出2);		
...		
如果 myVar 是 9, 那么我们在指定的区域内把指定的像素点用指定的颜色绘制(绘制出9);		
如果 myVar 是 0, 那么我们在指定的区域内把指定的像素点用指定的颜色绘制(绘制出0);		
```

听起来不错, 这样我们就可以让 `shader` 输出 `1~0` 10个数字了, 继续简化问题, 先从最简单的地方入手, 我们试着处理一下 `1`, 暂时不管 `myVar` 的值, 我们只是简单地在屏幕上绘制一个 `1`, 那么代码如下:

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

`4` 调试区设置为右上角
![4](https://static.oschina.net/uploads/img/201603/31014356_bx2t.png "在这里输入图片标题")

`2` 调试区设置为全屏
![2](https://static.oschina.net/uploads/img/201603/31014421_ZIH2.png "在这里输入图片标题")

`5` 调试区设置为全屏

![输入图片说明](https://static.oschina.net/uploads/img/201603/31014446_VeuH.png "在这里输入图片标题")


理论上来说, 有了上面这些基础, 我们就可以自由地通过 `shader` 输出要观察变量的值了, 不过貌似有一个 `bug`: 重复运行 `ledChar` 函数会导致花屏, 暂时还没搞清楚问题原因.

在显示数字的方向上, 今天还想到几种获得数字字型的办法, 一种是通过多个 `vec4` 或 `mat4` 来传, 另一种是通过 `texture` 来传, 大致考虑了下, 感觉还是通过 `texture` 传比较简单, 因为它是直接传图, 不需要再自己算像素点坐标了, 这个想法也准备后续试验一下.

另外, 今天在有了这个思路之后, 去搜索了一下 `shader debug`, 结果在 `StackOverflow` 网站发现也有人问同样的问题, 然后有人给出了本文提到的第一种简单方法, 还有人提出一种很有想象力的方案, 那就是把要观察的数据可视化, 直接用图像来表达数字, 看着挺有趣, [点击网页链接](http://stackoverflow.com/questions/2508818/how-to-debug-a-glsl-shader), 准备后面也试试. 

经过一番调试, 终于把刚刚能用的原型搞出来了:
[OpenGL ES 2.0 Shader 调试新思路(二): 做一个可用的原型](http://my.oschina.net/freeblues/blog/651680)
