#	OpenGL ES 2.0 Shader 调试新思路(二): 做一个可用的原型
	

#	目录

##	背景介绍

请参考前文[OpenGL ES 2.0 Shader 调试新思路(一): 改变提问方式](http://my.oschina.net/freeblues/blog/650307)

##	优化 ledChar 函数

前文中我们写了一个可以用来显示 `1~0` 10个数字字型的函数, 不过回头看看, 发现这个函数有些不太好阅读, 为方便讨论, 把该函数的代码拷贝在下面:

```
void ledChar(int,float,float,float,float);

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
        // 设置调试区背景色
        gl_FragColor = vec4(0.2,0.2,0.8,.5);
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
            gl_FragColor = vec4(0,1,0,.5);
        }
    }
}
```

代码完成后, 看着复杂的判断条件, 参差不齐的格式, 觉得不太好阅读, 忽然想到另一种实现 `LED` 字型的算法, 我们上面的算法是计算7段数码管每段的坐标范围, 然后绘制, 最多需要绘制5段(数字2,5,6,8,9), 判断语句写起来比较长, 其实我们可以反其道而行之, 意思就是说不画数字笔画, 改画数字笔画间的矩形, 因为仔细分析一下每个数字字型, 就会发现每个数字都可以用1个或2个小矩形分割出来, 上个示意图, 第一个示意图是传统的绘制方式, 第二个示意图是我们刚刚想到的用"矩形掩码", 或者叫"蒙版", 总之就这个意思:

`传统LED数字绘制原理图` VS. `新想到的利用"矩形掩码"绘制原理图`:

截图:

![示意图](https://static.oschina.net/uploads/img/201604/02012546_25t4.png "在这里输入图片标题")

为了方便起见, 写了一个计算矩形区域的辅助函数 `inRect`, 我们把新写的函数命名为 `ledRectChar`,  代码如下:

```
float x = vTexCoord.x;
float y = vTexCoord.y;

void ledRectChar(int,float,float,float,float);
bool inRect(float,float,float,float);

bool inRect(float x1,float x2, float y1, float y2){
    if(x>x1 && x<x2 && y>y1 && y<y2) { return true; } else { return false; }
}

void ledRectChar(int n, float xa,float xb, float ya, float yb){
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
        // 设置调试区背景色为绿色
        gl_FragColor = vec4(0.2,1.0,0.2,1.0);
        // 分别绘制出 LED 形式的数字 1~0 , 用黑色绘制1个或2个矩形,由矩形以外的绿色区域组成字型
        if((num==1 && (inRect(x1,ox-dx,y1,y2) || inRect(ox+dx,x2,y1,y2))) ||
           (num==2 && (inRect(x1,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2,y1+dy,oy-dy/2.0))) ||
           (num==3 && (inRect(x1,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==4 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2) || inRect(x1,x2-dx,y1,oy-dy/2.0))) ||
           (num==5 && (inRect(x1+dx,x2,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==6 && (inRect(x1+dx,x2,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2-dx,y1+dy,oy-dy))) ||
           (num==7 && inRect(x1,x2-dx,y1,y2-dy)) ||
           (num==8 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==9 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==0 && inRect(x1+dx,x2-dx,y1+dy,y2-dy))
          )
        {
            gl_FragColor = vec4(0,0,0,.5);
        }       
    }
}
```

这样考虑角度一变, 就发现其实用矩形在这种场景下更简单, 而且代码看起来清楚多了, 数字 `7` 和 `0` 用了一个矩形, 其他数字都用两个矩形就"掩"出来了(其实`1`用1个矩形就可以, 用两个是为了更美观一些)
.

另外, 貌似使用了太多的函数, 导致效率不高, 其实我也很乐意把这些函数全部都写成宏, 只是不太会写带参数的宏, 试了半天 `inRect`, 比如 

```
// ! 说明, 这是错的, 编译不通过
#define inRect(x1,x2,y1,y2) x>(x1)&&x<(x2)&&y>(y1)&&y<(y2)?true:false
```

结果老是有错误, 就没继续研究了.

##	改进为可用的原型

用了改进版的 `ledRectChar` 作为基础函数, 我们开始考虑实际的使用场景, 实际编程过程中 `shader` 用到的变量的值肯定不会只是一个一位整数, 所以我们首先得考虑多位整数, 其次还要考虑浮点数, 另外还要考虑负数的表示, 最后要考虑的是表示范围和准确度(这点最麻烦, 本文只是大致说一下).

列一下后续的需求清单:

*	多位正整数
*	多位浮点数
*	负数
*	给出表示范围和准确度

接下来我们一项一项来

###	表示多位正整数

在我发出前文 [OpenGL ES 2.0 Shader 调试新思路(一): 改变提问方式](http://my.oschina.net/freeblues/blog/650307) 后, 论坛上的一位朋友 `@dave1707` 用我们的基础函数 `ledChar` 写了一段表示多位正整数的代码, 并建议我把它完善一下, 首先表示感谢, 他的代码如下:

```
highp int nbr=8293;   // number to display

float m=0.96;
while (nbr>0)
{   m=m-0.015;
    int nn=nbr-((nbr/10)*10);
    ledChar(nn, m, 0.01, 0.96, 0.01);
    nbr=nbr/10;
}
```
也就是说多位正整数的需求已经解决, 这里存在一个存储精度的问题, 也就是当要表示的数字大于某个值时就会导致溢出, 这种情况我们不做太多处理, 主要是因为这里处理起来比较麻烦, 我们会在注释中说明本函数适用的数字范围.

接下来我们会在他的代码的基础上继续前进, 我们先分析一下 `多位浮点数` 和 `负数` 这两个需求, 发现它们一个需要 `小数点`, 一个需要`负号`, 也就是说在我们的基础函数 `ledRectChar` 中需要新增两种字型, 那么我们先来升级一下基础函数.

###	新增的两种字型:小数点和负号

先来处理`小数点`, 前面用变量 `num` 的值 `1~0` 分别表示 `1~0` 这10个数字的字型, 那么新增的`小数点`和负号分别用数字 `10` 和 `11` 表示, 然后用"矩形掩码"把它们的字型画出.

增加这么两行判断:

```
(num==10 && (inRect(x1,x2,oy-dy,y2) || inRect(x1,ox-dx*2.0,y1,oy-dy) || inRect(ox+dx*2.0,x2,y1,oy-dy) )) ||
(num==11 && (inRect(x1,x2,oy+dy,y2) || inRect(x1,x2,y1,oy-dy))) 
```

更新后的 `ledRectChar` 函数如下:

```
void ledRectChar(int n, float xa,float xb, float ya, float yb){
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
        // 设置调试区背景色
        gl_FragColor = vec4(0.2,1.0,0.2,1.0);
        // 分别绘制出 LED 形式的数字 1~0 , 用黑色绘制1个或2个矩形,由矩形以外的绿色区域组成字型
        if((num==1 && (inRect(x1,ox-dx,y1,y2) || inRect(ox+dx,x2,y1,y2))) ||
           (num==2 && (inRect(x1,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2,y1+dy,oy-dy/2.0))) ||
           (num==3 && (inRect(x1,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==4 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2) || inRect(x1,x2-dx,y1,oy-dy/2.0))) ||
           (num==5 && (inRect(x1+dx,x2,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==6 && (inRect(x1+dx,x2,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2-dx,y1+dy,oy-dy))) ||
           (num==7 && inRect(x1,x2-dx,y1,y2-dy)) ||
           (num==8 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==9 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==0 && inRect(x1+dx,x2-dx,y1+dy,y2-dy)) ||
           // 传入10则绘制小数点, 传入11则绘制负号, 传入12则清空
           (num==10 && (inRect(x1,x2,oy-dy,y2) || inRect(x1,ox-dx*2.0,y1,oy-dy) || inRect(ox+dx*2.0,x2,y1,oy-dy) )) ||
           (num==11 && (inRect(x1,x2,oy+dy,y2) || inRect(x1,x2,y1,oy-dy))) ||
           (num==12)
          )
        {
            gl_FragColor = vec4(0,0,0,.5);
        }       
    }
}
```

###	浮点数和负数

调试通过, OK, 现在基础函数已经可以提供全部字型了, 我们接着来实现浮点数的表示, 基于前面 `@dave1707` 的代码, 把浮点数先取绝对值(为了避免要区分正负数分别对应的`floor`和`ceil`两种取整方式), 再把绝对值分离出整数部分和小数部分, 然后都当做整数来处理, 按`个十百千万...`位插入一个20位的数组, 整数部分和小数部分中间插一个`小数点`所对应的数字`10`, 最后判断一下是不是负数, 是的话就插入`负号`所对应的数字`11`, 还有就是开始要初始化一下数组, 初始值置为 `12`(在基础函数中对应黑色背景), 否则数组默认值都是 `0`, 显示时会在空白的数字位全部显示为 `0`, 影响观感, 代码如下:

```
void showFloat(float f){
    int myNum[20];
    int k = 0;
    int iPart = int(floor(abs(f)));
    int fPart = int(fract(abs(f))*100000.0);
    float m=0.86;
    
    // 初始化数组,全部置为代表黑色的12
    for(int i=0; i<20; i++){
        myNum[i] = 12;
    }

    // 插入小数部分
    while (fPart>0)
    {   
        // 从个位开始, 依次取出个位,十位,百位,千位...的数字值
        myNum[k++]=fPart-((fPart/10)*10);
        fPart=fPart/10;
    }
    
    // 如果是0
    if(f==0.0){myNum[k++] = 0;}

    // 插入小数点
    myNum[k++] = 10;
    
    // 插入整数部分
    while (iPart>0)
    {   
        myNum[k++]=iPart-((iPart/10)*10);
        iPart=iPart/10;
    }
    
    // 如果是负数,则插入代表负号的11
    if(f<0.0) { myNum[k++]=11;}
   
    // 循环输出数字数组
    for(int i=0; i<20; i++)
    {
        m = m-0.03;
        ledRectChar(myNum[i], m, 0.02, 0.6, 0.15);
    }
    
}
```

很好, 调试通过, 基本搞定, 好像忘记处理负整数了, 为了避免麻烦, 我们可以建议用户把`负整数`进行强制类型转换为`负浮点数`, 就可以直接使用我们的 `showFloat` 函数了, 具体来说就是这么调用:


*    showFloat(float(-1234));


显示截图如下:
![-1234](https://static.oschina.net/uploads/img/201604/02012716_WSKI.png "在这里输入图片标题")


###	显示范围和准确度

最后说一下这个不得不说的问题, 很多编程语言都需要考虑一个数值表示范围, 尤其是浮点数, 比如 `shader` 里的浮点数就是数值越大, 小数位越少, 而且这时比较小的小数会被舍掉, 我们为小数部分留了5位, 整数部分留了13位, 当然, 如果你需要调试更大的数, 也可以自己修改数组的大小--不过好像 `shader` 中太大的数会返回溢出, 大家根据自己的需求看着办吧.

看看这几个截图:

*    showFloat(2097152.411);

![舍掉小数](https://static.oschina.net/uploads/img/201604/02012812_2yQ2.png "在这里输入图片标题")

*    showFloat(2097152.11);

![保留小数](https://static.oschina.net/uploads/img/201604/02012855_bQRE.png "在这里输入图片标题")


在此不得不赞叹一下我大 `Common Lisp` 的强悍, 毕竟能直接计算 `1024^1024` (1024的1024次方)的语言唯有我大  `Common Lisp` 了, 看看:

截图:
![大数字](https://static.oschina.net/uploads/img/201604/02012942_94EU.png "在这里输入图片标题")


##	可用原型的完整代码

现在我们基本完成一个可用原型了, 虽然效率不怎么样, `FPS`甚至降低了20倍(从60降低到3), 但是首先我们解决了有没有的问题, 好不好的问题就留待后面解决了, 如果有需求那就继续优化好了, 下面给出可用原型的全部代码:

###	shader代码

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

float x = vTexCoord.x;
float y = vTexCoord.y;

void ledChar(int,float,float,float,float);
void ledRectChar(int,float,float,float,float);
void showInt(int);
void showFloat(float);
bool inRect(float,float,float,float);

void main() {
    lowp vec4 col = texture2D( texture, vTexCoord ) * vColor;

	// 默认全部设置为黑色
    gl_FragColor = vec4(.1,.1,.1,1);

    showFloat(-.1111111);
    //showFloat(float(-9765));
    
}

void showFloat(float f){
    int myNum[20];
    int k = 0;
    int iPart = int(floor(abs(f)));
    int fPart = int(fract(abs(f))*100000.0);
    float m=0.86;
    
    // 初始化数组,全部置为代表黑色的12
    for(int i=0; i<20; i++){
        myNum[i] = 12;
    }

    // 插入小数部分
    while (fPart>0)
    {   
        // 从个位开始, 依次取出个位,十位,百位,千位...的数字值
        myNum[k++]=fPart-((fPart/10)*10);
        fPart=fPart/10;
    }
    
    // 如果是0
    if(f==0.0){myNum[k++] = 0;}

    // 插入小数点
    myNum[k++] = 10;
    
    // 插入整数部分
    while (iPart>0)
    {   
        myNum[k++]=iPart-((iPart/10)*10);
        iPart=iPart/10;
    }
    
    // 如果是负数,则插入代表负号的11
    if(f<0.0) { myNum[k++]=11;}
   
    // 循环输出数字数组
    for(int i=0; i<20; i++)
    {
        m = m-0.03;
        ledRectChar(myNum[i], m, 0.02, 0.6, 0.15);
    }    
}

bool inRect(float x1,float x2, float y1, float y2){
    if(x>x1 && x<x2 && y>y1 && y<y2) { return true; } else { return false; }
}

void ledRectChar(int n, float xa,float xb, float ya, float yb){
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
        // 设置调试区背景色
        gl_FragColor = vec4(0.2,1.0,0.2,1.0);
        // 分别绘制出 LED 形式的数字 1~0 , 用黑色绘制1个或2个矩形,由矩形以外的绿色区域组成字型
        if((num==1 && (inRect(x1,ox-dx,y1,y2) || inRect(ox+dx,x2,y1,y2))) ||
           (num==2 && (inRect(x1,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2,y1+dy,oy-dy/2.0))) ||
           (num==3 && (inRect(x1,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==4 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2) || inRect(x1,x2-dx,y1,oy-dy/2.0))) ||
           (num==5 && (inRect(x1+dx,x2,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==6 && (inRect(x1+dx,x2,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2-dx,y1+dy,oy-dy))) ||
           (num==7 && inRect(x1,x2-dx,y1,y2-dy)) ||
           (num==8 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1+dx,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==9 && (inRect(x1+dx,x2-dx,oy+dy/2.0,y2-dy) || inRect(x1,x2-dx,y1+dy,oy-dy/2.0))) ||
           (num==0 && inRect(x1+dx,x2-dx,y1+dy,y2-dy)) ||
           // 传入10则绘制小数点, 传入11则绘制负号, 传入12则清空
           (num==10 && (inRect(x1,x2,oy-dy,y2) || inRect(x1,ox-dx*2.0,y1,oy-dy) || inRect(ox+dx*2.0,x2,y1,oy-dy) )) ||
           (num==11 && (inRect(x1,x2,oy+dy,y2) || inRect(x1,x2,y1,oy-dy))) ||
           (num==12)
          )
        {
            gl_FragColor = vec4(0,0,0,.5);
        }       
    }
}

]]
}
```

###	配套Codea代码

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

##	后记

经过一番调试折腾, 终于完成一个刚刚能用的原型, 以后在 `Codea` 下调试 `shader` 程序起码有个工具勉强可用了. 当然, 这几个函数也可以用于调试其他平台的 `shader` 程序.

为了提高人类整体的工作效率, 我们后续会把这个原型发布到 `Github` 上, 以供其他需要观察 `shader` 内部变量的同学使用, 起个响亮的名字 `ShaderDebugger`:

[Github 地址](https://github.com/FreeBlues/ShaderDebugger)        
[OSChina 地址--还没写后面补充]()
