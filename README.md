# ShaderDebugger A tool for shader debugging

##  Intro

[`Codea`](codea.io) supports the coder to write and run the `OpenGL ES shader` code on `iPad` directlly, it is very useful for design `3D` program, but the vars in `shader` can not be wathched directlly, it makes the debug hard. Because of this I try to make a tool function to show the value of var in `shader` outpu th screen, a proptype, it is simple, bad performance, but can work.  With it we can watch the var in `shader` easily.

##  About the functions

In fact, they are a few global vars and functions:

* Use for the coordinate `float x` 和 `float y`
* Use for display the value of var `showFloat()`
* Use for generate the font `ledRectChar()`
* Use for judge the rect area `inRect()`

##  Usage:

Copy the global vars and the functions into the code of `fragment shader`, then run the `showFloat()`, like below:

### Show the int

Positive int

```
shoufloat(float(2345));
```

Negative int

```
shoufloat(float(2345));
```

Float

```
shoufloat(.2345));
```

```
shoufloat(1234.345);
```

```
shoufloat(-1234.345);
```


##  Other Info

Articles record the whole development: from an idea to a available propotype

[How to watch a var in shader? -A new solution: change the question from 'What is' to 'Is it'](https://github.com/FreeBlues/ShaderDebugger/blob/master/Record-Article-1.md)        
[]()

---

# ShaderDebugger 着色器调试工具

##  介绍

[`Codea`](codea.io) 支持在 `iPad` 上直接编写 `OpenGL ES` 的 `shader` 程序, 对于设计 `3D` 程序而已非常方便, 但是 `shader` 内部的变量无法直接观察, 使得调试起来比较困难. 鉴于此, 试着写了一个可以把 `shader` 内部变量值输出到屏幕的工具, 类似原型的东西, 比较粗糙, 性能很差, 不过刚好能用, 利用它可以轻松地观察 `shader` 内变量的值.

##  工具函数说明

实际上就是几个函数, 包括:

* 用于记录屏幕坐标的两个全局变量 `float x` 和 `float y`
* 用于最终显示变量值的 `showFloat()`
* 用于生成所需字型的 `ledRectChar()`
* 用于判断矩形区域范围的 `inRect()`

##  使用方法

把全局变量和该函数的声明和实现拷贝到 `fragment shader` 的代码中, 直接在 `main` 函数中调用即可, 如下所示:

### 显示整数

正整数

```
shoufloat(float(2345));
```

负整数
```
shoufloat(float(2345));
```

浮点数

```
shoufloat(.2345));
```

```
shoufloat(1234.345);
```

```
shoufloat(-1234.345);
```

##  其他

记录完成这个原型工具的整个开发调试过程的思路

[OpenGL ES 2.0 Shader 调试新思路(一): 改变提问方式-是什么(答案是具体值)与是不是(答案是布尔值)](https://github.com/FreeBlues/ShaderDebugger/blob/master/Record-Article-1.md) 
[OpenGL ES 2.0 Shader 调试新思路(二): 做一个可用的原型]()
