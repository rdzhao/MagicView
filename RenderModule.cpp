#include "RenderModule.h"

void RenderModule::genVertexBuffer()
{
	mVertex = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	mVertex->create();
	mVertex->bind();
	mVertex->setUsagePattern(QOpenGLBuffer::StaticDraw);
	mVertex->allocate(&vertexData[0], vertexData.size()*sizeof(float));
	//std::cout << vertexData.size() << std::endl;
}

void RenderModule::genIndexBuffer()
{
	mIndex = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	mIndex->create();
	mIndex->bind();
	mIndex->setUsagePattern(QOpenGLBuffer::StaticDraw);
	mIndex->allocate(&indexData[0], indexData.size()*sizeof(int));
	//std::cout << indexData.size() << std::endl;
}

void RenderModule::genNormalBuffer()
{
	mNormal = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	mNormal->create();
	mNormal->bind();
	mNormal->setUsagePattern(QOpenGLBuffer::StaticDraw);
	mNormal->allocate(&normalData[0], normalData.size()*sizeof(float));
}

void RenderModule::genColorBuffer()
{
	mColor = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	mColor->create();
	mColor->bind();
	mColor->setUsagePattern(QOpenGLBuffer::StaticDraw);
	mColor->allocate(&colorData[0], colorData.size()*sizeof(float));
}

void RenderModule::setLamp(float x, float y, float z, float power)
{
	lightPosID = mProgram->uniformLocation("LightPosition_worldspace");
	lightPowerID = mProgram->uniformLocation("LightPower");

	mProgram->setUniformValue(lightPosID, x, y, z);
	mProgram->setUniformValue(lightPowerID, GLfloat(power));
}

void RenderModule::setMatrix()
{
	mvpID = mProgram->uniformLocation("MVP");
	viewMatrixID = mProgram->uniformLocation("V");
	modelMatrixID = mProgram->uniformLocation("M");

	QMatrix4x4 modelMatrix, viewMatrix, projection, mvp;
	modelMatrix.setToIdentity();
	modelMatrix = modelMatrix;
	viewMatrix = camera.viewMatrix*camera.translationMatrix*camera.rotationMatrix;
	projection = camera.projectionMatrix;
	mvp = projection*viewMatrix*modelMatrix;

	mProgram->setUniformValue(mvpID, mvp);
	mProgram->setUniformValue(modelMatrixID, modelMatrix);
	mProgram->setUniformValue(viewMatrixID, viewMatrix);
}

void RenderModule::clear()
{
	mVertex->destroy();
	mIndex->destroy();
	mNormal->destroy();
	mColor->destroy();

	mVAO->destroy();

	delete mProgram;
}

/*******************************************\
		Class MeshModule
\*******************************************/

void MeshModule::setVertexShaderSource()
{
	vertexShaderSource =
	{
		"#version 330 core \n"

		"layout(location = 0) in vec3 vertexPosition_modelspace; \n"
		"layout(location = 1) in vec3 vertexColor; \n"
		"layout(location = 2) in vec3 vertexNormal_modelspace; \n"

		"out vec3 Position_worldspace; \n"
		"out vec3 Normal_cameraspace; \n"
		"out vec3 EyeDirection_cameraspace; \n"
		"out vec3 LightDirection_cameraspace; \n"
		"out vec3 fragmentColor; \n"

		"uniform mat4 MVP; \n"
		"uniform mat4 V; \n"
		"uniform mat4 M; \n"
		"uniform vec3 LightPosition_worldspace; \n"

		"void main(){ \n"
		"gl_Position =  MVP * vec4(vertexPosition_modelspace,1); \n"
		"Position_worldspace = ( M * vec4(vertexPosition_modelspace,1)).xyz; \n"

		"vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz; \n"
		"EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace; \n"

		"vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz; \n"
		"LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace; \n"

		"Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; \n"

		"fragmentColor = vertexColor; \n"
		"} \n"

	};
}

void MeshModule::setFragmentShaderSource()
{
	fragmentShaderSource =
	{
		"#version 330 core \n"

		"in vec3 Position_worldspace; \n"
		"in vec3 Normal_cameraspace; \n"
		"in vec3 EyeDirection_cameraspace; \n"
		"in vec3 LightDirection_cameraspace; \n"
		"in vec3 fragmentColor; \n"

		"out vec3 color; \n"

		"uniform vec3 LightPosition_worldspace; \n"
		"uniform float LightPower; \n"
		"uniform sampler2D textureSampler; \n"

		"void main(){ \n"
		"vec3 LightColor = vec3(1,1,1); \n"

		"vec3 MaterialDiffuseColor = fragmentColor; \n"
		"vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor; \n"
		"vec3 MaterialSpecularColor = vec3(0.1,0.1,0.1); \n"

		"float distance = length( LightPosition_worldspace - Position_worldspace ); \n"

		"vec3 n = normalize( Normal_cameraspace ); \n"
		"vec3 l = normalize( LightDirection_cameraspace ); \n"
		"float cosTheta = clamp( dot( n,l ), 0, 1 ); \n"

		"vec3 E = normalize(EyeDirection_cameraspace); \n"
		"vec3 R = reflect(-l,n); \n"
		"float cosAlpha = clamp( dot( E,R ), 0, 1 ); \n"

		"color =  \n"
		"MaterialAmbientColor + \n"
		"MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) + \n"
		"MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance); \n"
		"} \n"
	};
}

void MeshModule::genVertexArray()
{
	mVAO = new QOpenGLVertexArrayObject();
	mVAO->create();
	mVAO->bind();

	mProgram->enableAttributeArray(0);
	mVertex->bind();
	mIndex->bind();
	mProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

	mProgram->enableAttributeArray(1);
	mColor->bind();
	mProgram->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

	mProgram->enableAttributeArray(2);
	mNormal->bind();
	mProgram->setAttributeBuffer(2, GL_FLOAT, 0, 3, 0);
}

void MeshModule::init()
{
	setVertexShaderSource();
	setFragmentShaderSource();

	mProgram = new QOpenGLShaderProgram();
	mProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	mProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	mProgram->link();
	mProgram->bind();
}

void MeshModule::render()
{
	init();

	genVertexBuffer();
	genIndexBuffer();
	genColorBuffer();
	genNormalBuffer();
	genVertexArray();

	setLamp(0, 0, 20, 400);
	setMatrix();

	mProgram->bind();

	glDrawElements(GL_TRIANGLES, indexData.size(), GL_UNSIGNED_INT, 0);

	clear();
}

void MeshModule::setData(std::vector<float> vd, std::vector<int> id, std::vector<float> nd, std::vector<float> cd)
{
	vertexData = vd;
	indexData = id;
	normalData = nd;
	colorData = cd;
}

void MeshModule::setCamera(Camera c)
{
	camera = c;
}

/*******************************************\
			Class WireFrameModule
\*******************************************/

void WireFrameModule::setVertexShaderSource()
{
	vertexShaderSource =
	{
		"#version 330 core \n"

		"layout(location = 0) in vec3 vertexPosition_modelspace; \n"
		"layout(location = 1) in vec3 vertexColor; \n"
		"layout(location = 2) in vec3 vertexNormal_modelspace; \n"

		"out vec3 Position_worldspace; \n"
		"out vec3 Normal_cameraspace; \n"
		"out vec3 EyeDirection_cameraspace; \n"
		"out vec3 LightDirection_cameraspace; \n"
		"out vec3 fragmentColor; \n"

		"uniform mat4 MVP; \n"
		"uniform mat4 V; \n"
		"uniform mat4 M; \n"
		"uniform vec3 LightPosition_worldspace; \n"

		"void main(){ \n"
		"gl_Position =  MVP * vec4(vertexPosition_modelspace,1); \n"
		"Position_worldspace = ( M * vec4(vertexPosition_modelspace,1)).xyz; \n"

		"vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz; \n"
		"EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace; \n"

		"vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz; \n"
		"LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace; \n"

		"Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; \n"

		"fragmentColor = vertexColor; \n"
		"} \n"

	};
}

void WireFrameModule::setFragmentShaderSource()
{
	fragmentShaderSource =
	{
		"#version 330 core \n"

		"in vec3 Position_worldspace; \n"
		"in vec3 Normal_cameraspace; \n"
		"in vec3 EyeDirection_cameraspace; \n"
		"in vec3 LightDirection_cameraspace; \n"
		"in vec3 fragmentColor; \n"

		"out vec3 color; \n"

		"uniform vec3 LightPosition_worldspace; \n"
		"uniform float LightPower; \n"
		"uniform sampler2D textureSampler; \n"

		"void main(){ \n"
		"vec3 LightColor = vec3(1,1,1); \n"

		"vec3 MaterialDiffuseColor = fragmentColor; \n"
		"vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor; \n"
		"vec3 MaterialSpecularColor = vec3(0.1,0.1,0.1); \n"

		"float distance = length( LightPosition_worldspace - Position_worldspace ); \n"

		"vec3 n = normalize( Normal_cameraspace ); \n"
		"vec3 l = normalize( LightDirection_cameraspace ); \n"
		"float cosTheta = clamp( dot( n,l ), 0, 1 ); \n"

		"vec3 E = normalize(EyeDirection_cameraspace); \n"
		"vec3 R = reflect(-l,n); \n"
		"float cosAlpha = clamp( dot( E,R ), 0, 1 ); \n"

		"color =  \n"
		"MaterialAmbientColor + \n"
		"MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) + \n"
		"MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance); \n"
		"} \n"
	};
}

void WireFrameModule::setData(std::vector<float> vd, std::vector<int> id, std::vector<float> nd, std::vector<float> cd)
{
	vertexData = vd;
	indexData = id;
	normalData = nd;
	colorData = cd;
}

void WireFrameModule::setCamera(Camera c)
{
	camera = c;
}

void WireFrameModule::genVertexArray()
{
	mVAO = new QOpenGLVertexArrayObject();
	mVAO->create();
	mVAO->bind();

	mProgram->enableAttributeArray(0);
	mVertex->bind();
	mIndex->bind();
	mProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

	mProgram->enableAttributeArray(1);
	mColor->bind();
	mProgram->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

	mProgram->enableAttributeArray(2);
	mNormal->bind();
	mProgram->setAttributeBuffer(2, GL_FLOAT, 0, 3, 0);
}

void WireFrameModule::init()
{
	setVertexShaderSource();
	setFragmentShaderSource();

	mProgram = new QOpenGLShaderProgram();
	mProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	mProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	mProgram->link();
	mProgram->bind();
}

void WireFrameModule::render()
{
	init();

	genVertexBuffer();
	genIndexBuffer();
	genColorBuffer();
	genNormalBuffer();
	genVertexArray();

	setLamp(0, 0, 20, 400);
	setMatrix();

	mProgram->bind();

	glDrawElements(GL_LINES, indexData.size(), GL_UNSIGNED_INT, 0);

	clear();
}