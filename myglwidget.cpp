#include "myglwidget.h"
#include <QtDebug>


//GLSL3.0版本后,废弃了attribute关键字（以及varying关键字），属性变量统一用in/out作为前置关键字
#define GL_VERSION  "#version 330 core\n"

#define GLCHA(x)  #@x           //加单引号,将x变为字符
#define GLSTR(x)  #x            //加双引号，将x变为字符串
#define GET_GLSTR(x) GL_VERSION#x


const char* vsrc = GET_GLSTR(
    layout(location = 0) in vec3 aPos;
	layout(location = 1) in vec3 aColor;
	out vec3 ourColor;
	out vec2 TexCoord;
void main(void)
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = aTexCoord;
}
);

const char* fsrc = GET_GLSTR(
	out vec4 FragColor;
	in vec3 ourColor;
	in vec2 TexCoord;
	uniform sampler2D ourTexture;
void main(void)
{
	FragColor = texture(ourTexture, TexCoord);
}
);

myGlWidget::myGlWidget(QWidget* parent) :QOpenGLWidget(parent)
{
}

void myGlWidget::paintGL()
{
    // 绘制
   // glViewport(0, 0, width(), height());

    glClear(GL_COLOR_BUFFER_BIT);

    // 渲染Shader
    //program->bind(); //绑定激活Program对象
    vao.bind();      //绑定激活vao
	m_texture->bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);    //绘制3个定点,样式为三角形

	m_texture->release();
    vao.release();       //解绑
}

void myGlWidget::initializeGL()
{

    // 为当前环境初始化OpenGL函数
    initializeOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);    //设置背景色为白色


    //初始化纹理对象
    m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_texture->setData(QImage(":/bk_login.png"));
    m_texture->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Nearest);
    //设置缩小和放大的方式,缩小图片采用LinearMipMapLinear线性过滤,并使用多级渐远纹理邻近过滤,放大图片采用:Nearest邻近过滤
    m_texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    m_texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);


	//1.创建着色器程序
	program = new QOpenGLShaderProgram;
	program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
	program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
	program->link();
	program->bind();//激活Program对象


	//2.初始化VBO,将顶点数据存储到buffer中,等待VAO激活后才能释放
	float vertices[] = {
		// 位置               // 颜色               //纹理坐标
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,    2.0f, 0.0f,//   右下
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // 左下
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    1.0f, 2.0f  //  顶部
	};

	vbo.create();
	vbo.bind(); //绑定到当前的OpenGL上下文,
	vbo.allocate(vertices, sizeof(vertices));
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw); //设置为一次修改，多次使用


	//3.初始化VAO,设置顶点数据状态(顶点，法线，纹理坐标等)
	vao.create();
	vao.bind();

	// void setAttributeBuffer(int location, GLenum type, int offset, int tupleSize, int stride = 0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));   //设置aPos顶点属性
	program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 8 * sizeof(float));   //设置aColor顶点颜色
	program->setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(float), 2, 8 * sizeof(float));   //设置纹理坐标


	//offset:第一个数据的偏移量
	//tupleSize:一个数据有多少个元素,比如位置为xyz,颜色为rgb,所以是3
	//stride:步长,下个数据距离当前数据的之间距离,比如右下位置和左下位置之间间隔了:3个xyz值+3个rgb值,所以填入 6 * sizeof(float)

	program->enableAttributeArray(0); //使能aPos顶点属性
	program->enableAttributeArray(1); //使能aColor顶点颜色
	program->enableAttributeArray(2); //使能纹理坐标


	//4.解绑所有对象
	vao.release();
	vbo.release();


}
void myGlWidget::resizeEvent(QResizeEvent* e)
{


}
