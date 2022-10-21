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
	void slotShowYuv(uchar* ptr, uint width, uint height); //��ʾһ֡Yuvͼ��
protected:

	void paintGL();
	void initializeGL();
	void resizeEvent(QResizeEvent* e);
	//��дpaintEvent����
	void paintEvent(QPaintEvent* e);
private:
	QOpenGLShaderProgram* program;

	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vbo;
	QOpenGLTexture* m_texture = nullptr;
	QOpenGLTexture* m_texture2 = nullptr;

	GLuint textureUniformY, textureUniformU, textureUniformV;     //opengl��y��u��v����λ��
	QOpenGLTexture* textureY = nullptr, * textureU = nullptr, * textureV = nullptr;
	GLuint idY, idU, idV;             //�Լ��������������ID���������󷵻�0
	uint videoW, videoH;
	uchar* yuvPtr = nullptr;
};

#endif // MYGLWIDGET_H