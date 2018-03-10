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

void RenderModule::setCamera(Camera c)
{
	camera = c;
}

void RenderModule::setLightDistance(double d)
{
	lightDistance = d;
}

void RenderModule::setLamp()
{
	lightDisID = mProgram->uniformLocation("LightDistance");
	lightPowerID = mProgram->uniformLocation("LightPower");

	mProgram->setUniformValue(lightDisID, GLfloat(lightDistance));
	mProgram->setUniformValue(lightPowerID, GLfloat(0.7*lightDistance*lightDistance));
}

void RenderModule::setMatrix()
{
	mvpID = mProgram->uniformLocation("MVP");
	viewMatrixID = mProgram->uniformLocation("V");
	modelMatrixID = mProgram->uniformLocation("M");

	QMatrix4x4 modelMatrix, viewMatrix, projection, mvp;
	modelMatrix.setToIdentity();
	modelMatrix = modelMatrix;
	viewMatrix = camera.viewMatrix*camera.rotationMatrix*camera.translationMatrix;
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
		"out vec3 LightPosition_worldspace; \n"
		"out vec3 LightDirection_cameraspace; \n"
		"out vec3 fragmentColor; \n"

		"uniform mat4 MVP; \n"
		"uniform mat4 V; \n"
		"uniform mat4 M; \n"
		//"uniform vec3 LightPosition_worldspace; \n"
		"uniform float LightDistance; \n"

		"void main(){ \n"
		"gl_Position =  MVP * vec4(vertexPosition_modelspace,1); \n"
		"Position_worldspace = ( M * vec4(vertexPosition_modelspace,1)).xyz; \n"

		"vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz; \n"
		"EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace; \n"

		"vec4 origin=vec4(0,0,0,1); \n"
		"vec4 CameraPosition_worldspace=inverse(M)*inverse(V)*origin; \n"
		"LightPosition_worldspace=normalize(CameraPosition_worldspace.xyz)*LightDistance; \n"

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
		"in vec3 LightPosition_worldspace; \n"
		"in vec3 Normal_cameraspace; \n"
		"in vec3 EyeDirection_cameraspace; \n"
		"in vec3 LightDirection_cameraspace; \n"
		"in vec3 fragmentColor; \n"

		"out vec3 color; \n"

		//"uniform vec3 LightPosition_worldspace; \n"
		"uniform float LightDistance; \n"
		"uniform float LightPower; \n"
		"uniform sampler2D textureSampler; \n"

		"void main(){ \n"
		"vec3 LightColor = vec3(1,1,1); \n"

		"vec3 MaterialDiffuseColor = fragmentColor; \n"
		"vec3 MaterialAmbientColor = vec3(0.4,0.4,0.4) * MaterialDiffuseColor; \n"
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
	
	setLamp();
	setMatrix();
	
	mProgram->bind();

	//glDrawElements(GL_TRIANGLES, indexData.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 3);

	clear();
}

void MeshModule::setData(std::vector<float> vd, std::vector<int> id, std::vector<float> nd, std::vector<float> cd)
{
	vertexData = vd;
	indexData = id;
	normalData = nd;
	colorData = cd;
}

void MeshModule::appData(std::vector<float> vd, std::vector<int> id, std::vector<float> nd, std::vector<float> cd)
{
	vertexData.insert(vertexData.end(), vd.begin(), vd.end());
	indexData.insert(indexData.end(), id.begin(), id.end());
	normalData.insert(normalData.end(), nd.begin(), nd.end());
	colorData.insert(colorData.end(), cd.begin(), cd.end());
}

void MeshModule::delData(int begin, int length)
{
	vertexData.erase(vertexData.begin() + begin, vertexData.begin() + begin + length);
	normalData.erase(normalData.begin() + begin, normalData.begin() + begin + length);
	colorData.erase(colorData.begin() + begin, colorData.begin() + begin + length);
}

void MeshModule::highlightFace(int fidx, bool mark)
{
	if (mark){
		colorData[9 * fidx] = 0.6;
		colorData[9 * fidx + 1] = 0.2;
		colorData[9 * fidx + 2] = 0.2;
		colorData[9 * fidx + 3] = 0.6;
		colorData[9 * fidx + 4] = 0.2;
		colorData[9 * fidx + 5] = 0.2;
		colorData[9 * fidx + 6] = 0.6;
		colorData[9 * fidx + 7] = 0.2;
		colorData[9 * fidx + 8] = 0.2;
	}
	else{
		colorData[9 * fidx] = 0.5;
		colorData[9 * fidx + 1] = 0.5;
		colorData[9 * fidx + 2] = 0.5;
		colorData[9 * fidx + 3] = 0.5;
		colorData[9 * fidx + 4] = 0.5;
		colorData[9 * fidx + 5] = 0.5;
		colorData[9 * fidx + 6] = 0.5;
		colorData[9 * fidx + 7] = 0.5;
		colorData[9 * fidx + 8] = 0.5;
	}
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
		"out vec3 LightPosition_worldspace; \n"
		"out vec3 LightDirection_cameraspace; \n"
		"out vec3 fragmentColor; \n"

		"uniform mat4 MVP; \n"
		"uniform mat4 V; \n"
		"uniform mat4 M; \n"
		//"uniform vec3 LightPosition_worldspace; \n"
		"uniform float LightDistance; \n"

		"void main(){ \n"
		"gl_Position =  MVP * vec4(vertexPosition_modelspace,1); \n"
		"Position_worldspace = ( M * vec4(vertexPosition_modelspace,1)).xyz; \n"

		"vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz; \n"
		"EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace; \n"

		"vec4 origin=vec4(0,0,0,1); \n"
		"vec4 CameraPosition_worldspace=inverse(M)*inverse(V)*origin; \n"
		"LightPosition_worldspace=normalize(CameraPosition_worldspace.xyz)*LightDistance; \n"

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
		"in vec3 LightPosition_worldspace; \n"
		"in vec3 Normal_cameraspace; \n"
		"in vec3 EyeDirection_cameraspace; \n"
		"in vec3 LightDirection_cameraspace; \n"
		"in vec3 fragmentColor; \n"

		"out vec3 color; \n"

		//"uniform vec3 LightPosition_worldspace; \n"
		"uniform float LightDistance; \n"
		"uniform float LightPower; \n"
		"uniform sampler2D textureSampler; \n"

		"void main(){ \n"
		"vec3 LightColor = vec3(1,1,1); \n"

		"vec3 MaterialDiffuseColor = fragmentColor; \n"
		"vec3 MaterialAmbientColor = vec3(0.4,0.4,0.4) * MaterialDiffuseColor; \n"
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

void WireFrameModule::appData(std::vector<float> vd, std::vector<int> id, std::vector<float> nd, std::vector<float> cd)
{
	vertexData.insert(vertexData.end(), vd.begin(), vd.end());
	indexData.insert(indexData.end(), id.begin(), id.end());
	normalData.insert(normalData.end(), nd.begin(), nd.end());
	colorData.insert(colorData.end(), cd.begin(), cd.end());
}

void WireFrameModule::delData(int begin, int length)
{
	vertexData.erase(vertexData.begin() + begin, vertexData.begin() + begin + length);
	normalData.erase(normalData.begin() + begin, normalData.begin() + begin + length);
	colorData.erase(vertexData.begin() + begin, colorData.begin() + begin + length);
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

	setLamp();
	setMatrix();

	mProgram->bind();

	//glDrawElements(GL_LINES, indexData.size(), GL_UNSIGNED_INT, 0);
	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, vertexData.size() / 3);

	clear();
}

void WireFrameModule::highlightEdge(int eIdx, bool mark)
{
	eIdx = eIdx / 2;

	if (mark) {
		colorData[6 * eIdx] = 0.2;
		colorData[6 * eIdx + 1] = 0.6;
		colorData[6 * eIdx + 2] = 0.6;
		colorData[6 * eIdx + 3] = 0.2;
		colorData[6 * eIdx + 4] = 0.6;
		colorData[6 * eIdx + 5] = 0.6;
	}
	else{
		colorData[6 * eIdx] = 0.2;
		colorData[6 * eIdx + 1] = 0.2;
		colorData[6 * eIdx + 2] = 0.2;
		colorData[6 * eIdx + 3] = 0.2;
		colorData[6 * eIdx + 4] = 0.2;
		colorData[6 * eIdx + 5] = 0.2;
	}
}