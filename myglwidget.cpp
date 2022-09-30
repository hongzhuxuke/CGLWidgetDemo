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
void main(void)
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
}
);

const char* fsrc = GET_GLSTR(
	out vec4 FragColor;
	in vec3 ourColor;
void main(void)
{
	FragColor = vec4(ourColor, 1.0);
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
    glDrawArrays(GL_TRIANGLES, 0, 3);    //绘制3个定点,样式为三角形
    vao.release();       //解绑
    //program->release();  //解绑

}
void myGlWidget::initializeGL()
{

    // 为当前环境初始化OpenGL函数
    initializeOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);    //设置背景色为白色


	//1.创建着色器程序
	program = new QOpenGLShaderProgram;
	program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
	program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
	program->link();
	program->bind();//激活Program对象


	//2.初始化VBO,将顶点数据存储到buffer中,等待VAO激活后才能释放
	float vertices[] = {
		// 位置              // 颜色
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 右下 颜色对应红色(1.0f, 0.0f, 0.0f)
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 左下 颜色对应绿色
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // 顶部 颜色对应蓝色
	};

	vbo.create();
	vbo.bind(); //绑定到当前的OpenGL上下文,
	vbo.allocate(vertices, 18 * sizeof(GLfloat));
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw); //设置为一次修改，多次使用


	//3.初始化VAO,设置顶点数据状态(顶点，法线，纹理坐标等)
	vao.create();
	vao.bind();

	// void setAttributeBuffer(int location, GLenum type, int offset, int tupleSize, int stride = 0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(float)); //设置aPos顶点属性
	program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float)); //设置aColor顶点颜色
	//offset:第一个数据的偏移量
	//tupleSize:一个数据有多少个元素,比如位置为xyz,颜色为rgb,所以是3
	//stride:步长,下个数据距离当前数据的之间距离,比如右下位置和左下位置之间间隔了:3个xyz值+3个rgb值,所以填入 6 * sizeof(float)


	program->enableAttributeArray(0); //使能aPos顶点属性
	program->enableAttributeArray(1); //使能aColor顶点颜色


	//4.解绑所有对象
	vao.release();
	vbo.release();


}
void myGlWidget::resizeEvent(QResizeEvent* e)
{


}
