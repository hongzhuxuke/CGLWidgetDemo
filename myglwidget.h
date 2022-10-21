#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture> 

class myGlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:

	myGlWidget(QWidget* parent = nullptr);
	~myGlWidget();
public slots:
	void slotShowYuv(uchar* ptr, uint width, uint height); //显示一帧Yuv图像
protected:

	void paintGL();
	void initializeGL();
	void resizeEvent(QResizeEvent* e);
	//重写paintEvent方法
	void paintEvent(QPaintEvent* e);
private:
	QOpenGLShaderProgram* program;

	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vbo;
	QOpenGLTexture* m_texture = nullptr;
	QOpenGLTexture* m_texture2 = nullptr;

	GLuint textureUniformY, textureUniformU, textureUniformV;     //opengl中y、u、v分量位置
	QOpenGLTexture* textureY = nullptr, * textureU = nullptr, * textureV = nullptr;
	GLuint idY, idU, idV;             //自己创建的纹理对象ID，创建错误返回0
	uint videoW, videoH;
	uchar* yuvPtr = nullptr;
};

#endif // MYGLWIDGET_H