#include "myglwidget.h"
#include <QtDebug>
#define VERTEXIN 0
#define TEXTUREIN 1


//GLSL3.0版本后,废弃了attribute关键字（以及varying关键字），属性变量统一用in/out作为前置关键字
#define GL_VERSION  "#version 330 core\n"

#define GLCHA(x)  #@x           //加单引号,将x变为字符
#define GLSTR(x)  #x            //加双引号，将x变为字符串
#define GET_GLSTR(x) GL_VERSION#x

//顶点着色器源码
const char* vsrc =
"attribute vec4 vertexIn; \
    attribute vec2 textureIn; \
    varying vec2 textureOut;  \
    void main(void)           \
    {                         \
        gl_Position = vertexIn; \
        textureOut = textureIn; \
    }";

const char* fsrc =
#if defined(WIN32)        //windows下opengl es 需要加上float这句话
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
#endif  
"varying vec2 textureOut; \
    uniform sampler2D tex_y; \
    uniform sampler2D tex_u; \
    uniform sampler2D tex_v; \
    void main(void) \
    { \
        vec3 yuv; \
        vec3 rgb; \
        yuv.x = texture2D(tex_y, textureOut).r; \
        yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
        yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
        rgb = mat3( 1,       1,         1, \
                    0,       -0.39465,  2.03211, \
                    1.13983, -0.58060,  0) * yuv; \
        gl_FragColor = vec4(rgb, 1); \
    }";

myGlWidget::myGlWidget(QWidget* parent) :QOpenGLWidget(parent)
{
}
myGlWidget::~myGlWidget()
{
	makeCurrent();
	vbo.destroy();
	if(textureY)
		textureY->destroy();
	if(textureU)
		textureU->destroy();
	if(textureV)
		textureV->destroy();
	doneCurrent();
}

void myGlWidget::slotShowYuv(uchar* ptr, uint width, uint height)
{
	yuvPtr = ptr;
	videoW = width;
	videoH = height;
	update();    //调用update将执行paintEvent函数
}



void myGlWidget::paintGL()
{
	//------- 加载y数据纹理 ------- 
	glActiveTexture(GL_TEXTURE0);//激活纹理单元GL_TEXTURE0,系统里面的
	glBindTexture(GL_TEXTURE_2D, idY);//绑定y分量纹理对象id到激活的纹理单元
	//使用内存中的数据创建真正的y分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW, videoH, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//------- 加载u数据纹理 ------- 
	glActiveTexture(GL_TEXTURE1);//激活纹理单元GL_TEXTURE1
	glBindTexture(GL_TEXTURE_2D, idU);
	//使用内存中的数据创建真正的u分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW>>1, videoH>>1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr+videoW*videoH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//------- 加载v数据纹理 ------- 
	glActiveTexture(GL_TEXTURE2);//激活纹理单元GL_TEXTURE2
	glBindTexture(GL_TEXTURE_2D, idV);
	//使用内存中的数据创建真正的v分量纹理数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr+videoW*videoH*5/4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//指定y纹理要使用新值，只能用0，1，2等表示纹理单元的索引，这是opengl不人性化的地方
	glUniform1i(textureUniformY, 0);//0对应纹理单元GL_TEXTURE0, 指定y纹理要用新值
	glUniform1i(textureUniformU, 1);//1对应纹理单元GL_TEXTURE1, 指定u纹理要用新值
	glUniform1i(textureUniformV, 2);//2对应纹理单元GL_TEXTURE2, 指定v纹理要用新值
	//使用顶点数组方式绘制图形
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void myGlWidget::initializeGL()
{

    // 为当前环境初始化OpenGL函数
    initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);

  
	

	//2.初始化VBO,将顶点数据存储到buffer中,等待VAO激活后才能释放
	static const GLfloat vertices[]{
		// 位置顶点坐标          
		-1.0f,  -1.0f,   
		-1.0f, 1.0f,     
		1.0f, 1.0f,		 
		1.0f,  -1.0f,    
		//纹理坐标
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	vbo.create();
	vbo.bind(); //绑定到当前的OpenGL上下文,
	vbo.allocate(vertices, sizeof(vertices));

	//vbo.setUsagePattern(QOpenGLBuffer::StaticDraw); //设置为一次修改，多次使用

	//初始化顶点着色器
	QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vshader->compileSourceCode(vsrc);//编辑顶点着色器
	//初始化片段着色器 功能gpu中yuv转换成rgb
	QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment,this);
	fshader->compileSourceCode(fsrc);

	//1.创建着色器程序
	program = new QOpenGLShaderProgram;
	//program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
	//program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
	program->addShader(vshader);//将顶点着色器添加到程序容器
	program->addShader(fshader);//将片段着色器添加到程序容器
	//绑定属性vertexIn到指定位置ATTRIB_VERTEX,该属性在顶点着色源码其中有声明
	program->bindAttributeLocation("vertexIn", VERTEXIN);
	//绑定属性textureIn到指定位置ATTRIB_TECTURE,该属性在顶点着色源码其中有声明
	program->bindAttributeLocation("textureIn", TEXTUREIN);
	program->link();//连接所有添入到的着色器程序
	program->bind();//激活所有连接

	program->enableAttributeArray(VERTEXIN);//使能aPos顶点属性
	program->enableAttributeArray(TEXTUREIN);//使能aColor顶点颜色
	program->setAttributeBuffer(VERTEXIN,	GL_FLOAT, 0,					2, 2*sizeof(GLfloat));
	//offset:第一个数据的偏移量
	//tupleSize:一个数据有多少个元素,比如位置为xyz,颜色为rgb,所以是3
	//stride:步长,下个数据距离当前数据的之间距离,比如右下位置和左下位置之间间隔了:3个xyz值+3个rgb值,所以填入 6 * sizeof(float)
	program->setAttributeBuffer(TEXTUREIN,	GL_FLOAT, 8*sizeof(GLfloat),	2, 2*sizeof(GLfloat));

	//读取着色器中的数据变量tex_y,tex_u,tex_v的位置，这些变量的声明
	//可以在片段着色器源码中可以看到
	textureUniformY = program->uniformLocation("tex_y");
	textureUniformU = program->uniformLocation("tex_u");
	textureUniformV = program->uniformLocation("tex_v");

	textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
	textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);

	textureY->create();
	textureU->create();
	textureV->create();

	idY = textureY->textureId();	//获取返回y分量的纹理索引值
	idU = textureU->textureId();	//获取返回u分量的纹理索引值
	idV = textureV->textureId();	//获取返回v分量的纹理索引值
	glClearColor(0.0, 0.0, 0.0, 0.0);//设置背景为黑色
}
void myGlWidget::resizeEvent(QResizeEvent* e)
{


}
