# Computer-Graphics-with-OpenGL
SJTU计算机图形学 大作业 By Biscuitsword

Opengl入门作品，基于freeglut进行开发，并未使用Qt所以界面有些简陋。

以贝塞尔曲线(Bézier curve)为主要绘制结构搭建的简易编辑场景，tab可以切换到编辑轮廓曲线从而编辑物体。

直接使用一块完整的贝塞尔曲面绘制封闭图形存在一定困难使用近似法将曲面近似为四分之一圆（用魔数计算一组控制点），从而通过四块曲面拼接成一个旋转体。

因为邻近期末赶工完成，从交互设计的角度来说确实存在不少缺憾，好在主要功能都比较完全。

下载Release版本即可运行，同时有提供完整项目。

简单操作：
1.	键盘交互功能：

键盘交互主要为调整参数以及页面切换的快捷键。

Tab为二维编辑页面和三位编辑页面的切换。

X/Y/Z 为三维页面中，选中物体进行X、Y、Z轴的正向移动。

Alt + X/Y/Z 为三维页面中，选中物体进行X、Y、Z轴的逆向移动。

Q 为三维页面中，环境光的逆时针移动。

E 为三维页面中，环境光的顺时针移动。

R/G/B 为三维页面中，环境光r、g、b值的正向调整。

Alt + R/G/B 为三维页面中，环境光r、g、b值的负向调整。

S 为三维页面中，选中物体的放大。

Alt + S 为三维页面中，选中物体的缩小。

D 为三维页面中，删除选中物体。

2.	鼠标交互功能：

在二维页面中，点击并拖动控制点可以编辑曲线。

在二维页面中，滑动滚轮可以缩放页面。

在三维页面中，点击物体可以选中物体。

在三维页面中，右键可以呼出菜单。

在三维页面中，滑动滚轮可以调整视角距离，Alt + 鼠标滚轮可以修改光源高度 

3.	右键菜单交互功能；

模型功能。包括导入导出以及导入预设。

纹理功能。包括使用预设纹理，以及导入并使用纹理。

粒子系统功能。包括粒子系统切换和开关。

天空盒功能。包括切换天空盒。



