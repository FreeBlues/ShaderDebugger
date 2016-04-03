// fragment shader 代码
precision highp float;
uniform lowp sampler2D texture;
varying lowp vec4 vColor;
varying highp vec2 vTexCoord;

bool inRect(float,float,float,float);
void ledRectChar(int,float,float,float,float);
void showFloat(float);

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
    if(vTexCoord.x>x1 && vTexCoord.x<x2 && vTexCoord.y>y1 && vTexCoord.y<y2) 
    { return true; } else { return false; }
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
    if(inRect(x1,x2,y1,y2)) {
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
