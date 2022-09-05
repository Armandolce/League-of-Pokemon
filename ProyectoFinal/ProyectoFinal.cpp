/*
Semestre 2022-2
PROYECTO FINAL: LEAGUE OF POKEMON
*/

#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>
#include <irrKlang/irrKlang.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

using namespace irrklang;

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera [3];

int numCam;

bool pj;
bool prueba = true;
float rotExtremidades = 0.0f;
float red, green, blue;

float periodoDN;
bool Ciclo;
float valorCiclo = 0.7f;
int SkyboxSelector;

float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;

//Texturas
Texture trainerTexture;

//Modelos
Model PikaCuerpo;
Model PikaPata;
Model CentroP;
Model Escenario;
Model Freljord;
Model PoroCuerpo;
Model PoroPata;
Model Arbol;
Model Portal;
Model BattleStage;
Model Letrero1, Letrero2;
Model CasaPokemon;
Model Antorcha;
Model Poste;
Model DragoniteB;
Model DragoniteA;
Model DragoniteW1;
Model DragoniteW2;
Model Pokeball;
Model Bulbasaur, Charmander, Squirtle, slowpoke;
Model Pino;
Model Cabania;
Model Humo;
Model Espada;
Model Hacha;
Model Arco;
Model Flecha;

Skybox skybox;
Skybox skybox2;
Skybox skybox3;

//materiales
Material Material_brillante;
Material Material_opaco;


GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

void inputKeyframes(bool* keys);
void movPJ(bool* keys);


//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CreateAvatar() {
	unsigned int indicesPrimitivas[] = {

		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

	};

	GLfloat verticesLeg1[] = {
		//front
		//x		y		z		S		T			NX		NY		NZ
		0.0f,	0.0f,  0.0f,	0.0625f,0.5f,		0.0f,	0.0f,	-1.0f,	//0
		4.0f,	0.0f,  0.0f,	0.125f,	0.5f,		0.0f,	0.0f,	-1.0f,	//1
		4.0f,	12.0f,  0.0f,	0.125f,	0.6875f,	0.0f,	0.0f,	-1.0f,	//2
		0.0f,	12.0f,  0.0f,	0.0625f,0.6875f,	0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		4.0f,	0.0f,  0.0f,	0.125f, 0.5f,		-1.0f,	0.0f,	0.0f,
		4.0f,	0.0f, -4.0f,	0.1875f,0.5f,		-1.0f,	0.0f,	0.0f,
		4.0f,	12.0f, -4.0f,	0.1875f,0.6875f,	-1.0f,	0.0f,	0.0f,
		4.0f,  12.0f,	0.0f,	0.125f,	0.6875f,	-1.0f,	0.0f,	0.0f,

		// back
		0.0f,	0.0f, -4.0f,	0.25f, 0.5f,		0.0f,	0.0f,	1.0f,
		4.0f,	0.0f, -4.0f,	0.1875f,0.5f,		0.0f,	0.0f,	1.0f,
		4.0f,  12.0f, -4.0f,	0.1875f,0.6875f,	0.0f,	0.0f,	1.0f,
		0.0f, 12.0f, -4.0f,		0.25f,0.6875f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.0f,  0.5f,		1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.0625f,0.5f,		1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  0.0f,	0.0625f,0.6875f,	1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  -4.0f,	0.0f,	0.6875f,	1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.125f,	0.75f,		0.0f,	1.0f,	0.0f,
		4.0f,	0.0f,  -4.0f,	0.1875f,0.75f,		0.0f,	1.0f,	0.0f,
		4.0f,	0.0f,  0.0f,	0.1875f,0.6875f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.125f,	0.6875f,	0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 0.0f,	12.0f,  0.0f,	0.0625f, 0.75f,		0.0f,	-1.0f,	0.0f,
		 4.0f,	12.0f,  0.0f,	0.125f,	 0.75f,		0.0f,	-1.0f,	0.0f,
		 4.0f,	12.0f,	-4.0f,	0.125f,	0.6875f,	0.0f,	-1.0f,	0.0f,
		 0.0f,	12.0f,	-4.0f,	0.0625f,0.6875f,	0.0f,	-1.0f,	0.0f,
	};

	GLfloat verticesLeg2[] = {
		//front
		//x		y		z		S		T			NX		NY		NZ
		0.0f,	0.0f,  0.0f,	0.3125f, 0.0f,		0.0f,	0.0f,	-1.0f,	//0
		4.0f,	0.0f,  0.0f,	0.375f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		4.0f,	12.0f,  0.0f,	0.375f,	0.1875f,	0.0f,	0.0f,	-1.0f,	//2
		0.0f,	12.0f,  0.0f,	0.3125f,0.1875f,	0.0f,	0.0f,	-1.0f,	//3

		// right
		//x		y		z		S		T
		4.0f,	0.0f,  0.0f,	0.375f, 0.0f,		-1.0f,	0.0f,	0.0f,
		4.0f,	0.0f, -4.0f,	0.4375f,0.0f,		-1.0f,	0.0f,	0.0f,
		4.0f,	12.0f, -4.0f,	0.4375f,0.1875f,	-1.0f,	0.0f,	0.0f,
		4.0f,  12.0f,	0.0f,	0.375f,	0.1875f,	-1.0f,	0.0f,	0.0f,

		// back
		0.0f,	0.0f, -4.0f,	0.5f,  0.0f,		0.0f,	0.0f,	1.0f,
		4.0f,	0.0f, -4.0f,	0.4375f,0.0f,		0.0f,	0.0f,	1.0f,
		4.0f,  12.0f, -4.0f,	0.4375f,0.1875f,	0.0f,	0.0f,	1.0f,
		0.0f, 12.0f, -4.0f,		0.5f,0.1875f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.25f,  0.0f,		1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.3125f,0.0f,		1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  0.0f,	0.3125f,0.1875f,	1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  -4.0f,	0.25f,	0.1875f,	1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.375f,	0.1875f,	0.0f,	1.0f,	0.0f,
		4.0f,	0.0f,  -4.0f,	0.4375f,0.1875f,	0.0f,	1.0f,	0.0f,
		4.0f,	0.0f,  0.0f,	0.4375f,0.25f,		0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.375f,	0.25f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 0.0f,	12.0f,  0.0f,	0.3125f, 0.1875f,	0.0f,	-1.0f,	0.0f,
		 4.0f,	12.0f,  0.0f,	0.375f,	0.1875f,	0.0f,	-1.0f,	0.0f,
		 4.0f,	12.0f,	-4.0f,	0.375f,	0.25f,		0.0f,	-1.0f,	0.0f,
		 0.0f,	12.0f,	-4.0f,	0.3125f,0.25f,		0.0f,	-1.0f,	0.0f,
	};

	GLfloat verticesBody[] = {
		//front
		//x		y		z		S		T			NX		NY		NZ
		0.0f,	0.0f,  0.0f,	0.3125f, 0.5f,		0.0f,	0.0f,	-1.0f,	//0
		8.0f,	0.0f,  0.0f,	0.4375f, 0.5f,		0.0f,	0.0f,	-1.0f,	//1
		8.0f,	12.0f,  0.0f,	0.4375f, 0.6875f,	0.0f,	0.0f,	-1.0f,	//2
		0.0f,	12.0f,  0.0f,	0.3125f, 0.6875f,	0.0f,	0.0f,	-1.0f,	//3

		// right
		//x		y		z		S		T
		8.0f,	0.0f,  0.0f,	0.4375f, 0.5f,		-1.0f,	0.0f,	0.0f,
		8.0f,	0.0f, -4.0f,	0.5f,	0.5f,		-1.0f,	0.0f,	0.0f,
		8.0f,	12.0f, -4.0f,	0.5f,	0.6875f,	-1.0f,	0.0f,	0.0f,
		8.0f,  12.0f,	0.0f,	0.4375f,0.6875f,	-1.0f,	0.0f,	0.0f,

		// back
		0.0f,	0.0f, -4.0f,	0.5f,  0.5f,		0.0f,	0.0f,	1.0f,
		8.0f,	0.0f, -4.0f,	0.625f,	0.5f,		0.0f,	0.0f,	1.0f,
		8.0f,  12.0f, -4.0f,	0.625f,	0.6875f,	0.0f,	0.0f,	1.0f,
		0.0f, 12.0f, -4.0f,		0.5f,	0.6875f,	0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.25f,  0.5f,		1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.3125f,0.5f,		1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  0.0f,	0.3125f,0.6875f,	1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  -4.0f,	0.25f,	0.6875f,	1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.4375f, 0.75f,		0.0f,	1.0f,	0.0f,
		8.0f,	0.0f,  -4.0f,	0.5625f, 0.75f,		0.0f,	1.0f,	0.0f,
		8.0f,	0.0f,  0.0f,	0.5625f, 0.6875f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.4375f, 0.6875f,	0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 0.0f,	12.0f,  0.0f,	0.3125f, 0.6875f,	0.0f,	-1.0f,	0.0f,
		 8.0f,	12.0f,  0.0f,	0.4375f, 0.6875f,	0.0f,	-1.0f,	0.0f,
		 8.0f,	12.0f,	-4.0f,	0.4375f, 0.75f,		0.0f,	-1.0f,	0.0f,
		 0.0f,	12.0f,	-4.0f,	0.3125f, 0.75f,		0.0f,	-1.0f,	0.0f,
	};

	GLfloat verticesArm1[] = {
		//front
		//x		y		z		S		T			NX		NY		NZ
		0.0f,	0.0f,  0.0f,	0.6875f,0.5f,		0.0f,	0.0f,	-1.0f,	//0
		3.0f,	0.0f,  0.0f,	0.73437f,0.5f,		0.0f,	0.0f,	-1.0f,	//1
		3.0f,	12.0f,  0.0f,	0.73437f,0.6875f,	0.0f,	0.0f,	-1.0f,	//2
		0.0f,	12.0f,  0.0f,	0.6875f,0.6875f,	0.0f,	0.0f,	-1.0f,	//3

		// right
		//x		y		z		S		T
		3.0f,	0.0f,  0.0f,	0.7187f,0.5f,		-1.0f,	0.0f,	0.0f,
		3.0f,	0.0f, -4.0f,	0.7812f,0.5f,		-1.0f,	0.0f,	0.0f,
		3.0f,	12.0f, -4.0f,	0.7812f,0.6875f,	-1.0f,	0.0f,	0.0f,
		3.0f,  12.0f,	0.0f,	0.7187f,0.6875f,	-1.0f,	0.0f,	0.0f,

		// back
		0.0f,	0.0f, -4.0f,	0.84375f,0.5f,		0.0f,	0.0f,	1.0f,
		3.0f,	0.0f, -4.0f,	0.79687f,0.5f,		0.0f,	0.0f,	1.0f,
		3.0f,  12.0f, -4.0f,	0.79687f,0.6875f,	0.0f,	0.0f,	1.0f,
		0.0f, 12.0f, -4.0f,		0.84375f,0.6875f,	0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.625f, 0.5f,		1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.6875f,0.5f,		1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  0.0f,	0.6875f,0.6875f,	1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  -4.0f,	0.625f,	0.6875f,	1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.73437f,0.6875f,	0.0f,	1.0f,	0.0f,
		3.0f,	0.0f,  -4.0f,	0.7656f,0.6875f,	0.0f,	1.0f,	0.0f,
		3.0f,	0.0f,  0.0f,	0.7656f,0.75f,		0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.73437f,0.75f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 0.0f,	12.0f,  0.0f,	0.6875f,0.6875f,	0.0f,	-1.0f,	0.0f,
		 3.0f,	12.0f,  0.0f,	0.73437f,0.6875f,	0.0f,	-1.0f,	0.0f,
		 3.0f,	12.0f,	-4.0f,	0.73437f,0.75f,		0.0f,	-1.0f,	0.0f,
		 0.0f,	12.0f,	-4.0f,	0.6875f,0.75f,		0.0f,	-1.0f,	0.0f,
	};

	GLfloat verticesArm2[] = {
		//front
		//x		y		z		S		T			NX		NY		NZ
		0.0f,	0.0f,  0.0f,	0.5625f,0.0f,		0.0f,	0.0f,	-1.0f,	//0
		3.0f,	0.0f,  0.0f,	0.6093f,0.0f,		0.0f,	0.0f,	-1.0f,	//1
		3.0f,	12.0f,  0.0f,	0.6093f,0.1875f,	0.0f,	0.0f,	-1.0f,	//2
		0.0f,	12.0f,  0.0f,	0.5625f,0.1875f,	0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		3.0f,	0.0f,  0.0f,	0.6093f,  0.0f,		-1.0f,	0.0f,	0.0f,
		3.0f,	0.0f, -4.0f,	0.6718f,0.0f,		-1.0f,	0.0f,	0.0f,
		3.0f,	12.0f, -4.0f,	0.6718f,0.1875f,	-1.0f,	0.0f,	0.0f,
		3.0f,  12.0f,	0.0f,	0.6093f,0.1875f,	-1.0f,	0.0f,	0.0f,
		// back
		0.0f,	0.0f, -4.0f,	0.7185f,0.0f,		0.0f,	0.0f,	1.0f,
		3.0f,	0.0f, -4.0f,	0.6318f,0.0f,		0.0f,	0.0f,	1.0f,
		3.0f,  12.0f, -4.0f,	0.6318f,0.1875f,	0.0f,	0.0f,	1.0f,
		0.0f, 12.0f, -4.0f,		0.7185f,0.1875f,	0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.5f,  0.0f,		1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.5625f,0.0f,		1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  0.0f,	0.5625f,0.1875f,	1.0f,	0.0f,	0.0f,
		0.0f,  12.0f,  -4.0f,	0.5f,	0.1875f,	1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		0.0f,	0.0f,  -4.0f,	0.6093f,0.1875f,	0.0f,	1.0f,	0.0f,
		3.0f,	0.0f,  -4.0f,	0.6562f,0.1875f,	0.0f,	1.0f,	0.0f,
		3.0f,	0.0f,  0.0f,	0.6562f,0.25f,		0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.6093f,0.25f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 0.0f,	12.0f,  0.0f,	0.5625f,0.1875f,	0.0f,	-1.0f,	0.0f,
		 3.0f,	12.0f,  0.0f,	0.6093f,0.1875f,	0.0f,	-1.0f,	0.0f,
		 3.0f,	12.0f,	-4.0f,	0.6093f,0.25f,		0.0f,	-1.0f,	0.0f,
		 0.0f,	12.0f,	-4.0f,	0.5625f,0.25f,		0.0f,	-1.0f,	0.0f,
	};

	GLfloat verticesHead[] = {
		//front
		//x		y		z		S		T			NX		NY		NZ
		0.0f,	0.0f,  0.0f,	0.125f,  0.75f,		0.0f,	0.0f,	-1.0f,	//0
		8.0f,	0.0f,  0.0f,	0.25f,	0.75f,		0.0f,	0.0f,	-1.0f,	//1
		8.0f,	8.0f,  0.0f,	0.25f,	0.875f,		0.0f,	0.0f,	-1.0f,	//2
		0.0f,	8.0f,  0.0f,	0.125f,	0.875f,		0.0f,	0.0f,	-1.0f,	//3

		// right
		//x		y		z		S		T
		8.0f,	0.0f,  0.0f,	0.25f,  0.75f,		-1.0f,	0.0f,	0.0f,
		8.0f,	0.0f, -8.0f,	0.375f,	0.75f,		-1.0f,	0.0f,	0.0f,
		8.0f,	8.0f, -8.0f,	0.375f,	0.875f,		-1.0f,	0.0f,	0.0f,
		8.0f,  8.0f,	0.0f,	0.25f,	0.875f,		-1.0f,	0.0f,	0.0f,

		// back
		0.0f,	0.0f, -8.0f,	0.375f,  0.75f,		0.0f,	0.0f,	1.0f,
		8.0f,	0.0f, -8.0f,	0.5f,	0.75f,		0.0f,	0.0f,	1.0f,
		8.0f,	8.0f, -8.0f,	0.5f,	0.875f,		0.0f,	0.0f,	1.0f,
		0.0f, 8.0f, -8.0f,		0.375f,	0.875f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		0.0f,	0.0f,  -8.0f,	0.0f,  0.75f,		1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.125f,	0.75f,		1.0f,	0.0f,	0.0f,
		0.0f,	8.0f,  0.0f,	0.125f,	0.875f,		1.0f,	0.0f,	0.0f,
		0.0f,	8.0f, -8.0f,	0.0f,	0.875f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		0.0f,	0.0f,  -8.0f,	0.25f,  1.0f,		0.0f,	1.0f,	0.0f,
		8.0f,	0.0f,  -8.0f,	0.375f,	1.0f,		0.0f,	1.0f,	0.0f,
		8.0f,	0.0f,  0.0f,	0.375f,	0.875f,		0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,  0.0f,	0.25f,	0.875f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 0.0f,	8.0f,  0.0f,	0.125f,  0.875f,		0.0f,	-1.0f,	0.0f,
		 8.0f,	8.0f,  0.0f,	0.25f,	0.875f,		0.0f,	-1.0f,	0.0f,
		 8.0f,	8.0f,	-8.0f,	0.25f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 0.0f,	8.0f,	-8.0f,	0.125f,	1.0f,		0.0f,	-1.0f,	0.0f,
	};


	Mesh* Leg1 = new Mesh();
	Leg1->CreateMesh(verticesLeg1, indicesPrimitivas, 192, 36);
	meshList.push_back(Leg1);

	Mesh* Leg2 = new Mesh();
	Leg2->CreateMesh(verticesLeg2, indicesPrimitivas, 192, 36);
	meshList.push_back(Leg2);

	Mesh* Body = new Mesh();
	Body->CreateMesh(verticesBody, indicesPrimitivas, 192, 36);
	meshList.push_back(Body);

	Mesh* Arm1 = new Mesh();
	Arm1->CreateMesh(verticesArm1, indicesPrimitivas, 192, 36);
	meshList.push_back(Arm1);

	Mesh* Arm2 = new Mesh();
	Arm2->CreateMesh(verticesArm2, indicesPrimitivas, 192, 36);
	meshList.push_back(Arm2);

	Mesh* Head = new Mesh();
	Head->CreateMesh(verticesHead, indicesPrimitivas, 192, 36);
	meshList.push_back(Head);
}

///////////////////////////////KEYFRAMES/////////////////////

bool animacion = false;
bool luz = false;

glm::mat4 aux;

//variables para animación
float rotAvatar;
float movPikaz;
float movPikax;
float movPikay;
float movOffset;
float giroPika;
float betha;
bool dir;
float radioA;
float angulo;
float centrox;
float centroz;
float angOffset;
float puntox;
float puntoz;
float movNubes;
float rotAlaD;
float rotAlaI;
bool dirAlas;
float poroPataD;
float poroPataI;
bool poroPatadir;
bool movPoroPata;
float pikaPataD;
float pikaPataI;
bool movPikaPata;
bool pikaPatadir;

//NEW// Keyframes
float posXporo = 55.0, posYporo = 5.0, posZporo = 55.0;
float movPoro_x = 0.0f, movPoro_y = 0.0f, movPoro_z;
float rotPoro;
bool movPika;

#define MAX_FRAMES 60
int i_max_steps = 90;
int i_curr_steps = 5;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movPoro_x;		//Variable para PosicionX
	float movPoro_y;		//Variable para PosicionY
	float movPoro_z;		//Variable para PosicionZ
	float movPoro_xInc;		//Variable para IncrementoX
	float movPoro_yInc;		//Variable para IncrementoY
	float movPoro_zInc;		//Variable para IncrementoZ
	float rotPoro;
	float rotPoroInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 53;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	printf("\nFrameindex: %d\n", FrameIndex);
	printf("\nSteps: %d\n", i_curr_steps);


	KeyFrame[FrameIndex].movPoro_x = movPoro_x;
	KeyFrame[FrameIndex].movPoro_y = movPoro_y;
	KeyFrame[FrameIndex].movPoro_z = movPoro_z;
	KeyFrame[FrameIndex].rotPoro = rotPoro;

	FrameIndex++;
	i_curr_steps++;
}

void resetElements(void)
{

	movPoro_x = KeyFrame[0].movPoro_x;
	movPoro_y = KeyFrame[0].movPoro_y;
	movPoro_z = KeyFrame[0].movPoro_z;
	rotPoro = KeyFrame[0].rotPoro;
}

void interpolation(void)
{
	KeyFrame[playIndex].movPoro_xInc = (KeyFrame[playIndex + 1].movPoro_x - KeyFrame[playIndex].movPoro_x) / i_max_steps;
	KeyFrame[playIndex].movPoro_yInc = (KeyFrame[playIndex + 1].movPoro_y - KeyFrame[playIndex].movPoro_y) / i_max_steps;
	KeyFrame[playIndex].movPoro_zInc = (KeyFrame[playIndex + 1].movPoro_z - KeyFrame[playIndex].movPoro_z) / i_max_steps;
	KeyFrame[playIndex].rotPoroInc = (KeyFrame[playIndex + 1].rotPoro - KeyFrame[playIndex].rotPoro) / i_max_steps;

}

void animate(void)
{
	//Movimiento del objeto
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("playindex [%d](x,y,rotacion): (%f,%f.%f)\n", playIndex, movPoro_x, movPoro_y, movPoro_z);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			movPoro_x += KeyFrame[playIndex].movPoro_xInc;
			movPoro_y += KeyFrame[playIndex].movPoro_yInc;
			movPoro_z += KeyFrame[playIndex].movPoro_zInc;
			rotPoro += KeyFrame[playIndex].rotPoroInc;
			i_curr_steps++;
		}

	}
}

/* FIN KEYFRAMES*/

int main()
{
	mainWindow = Window(1980, 1080); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	rotAvatar = 0.0f;
	movPikaz = 0.0f;
	movPikax = 0.0f;
	movPikay = 0.0f;
	movOffset = 1.5f;
	giroPika = 0.0f;
	puntox = 225.0f;
	puntoz = 25.0f;
	movNubes = 0.0f;
	rotAlaD = 0.0f;
	rotAlaI = 0.0f;
	dirAlas = true;

	poroPataD = 0.0f;
	poroPataI = 0.0f;
	poroPatadir = false;
	movPoroPata = false;

	pikaPataD = 0.0f;
	pikaPataI = 0.0f;
	pikaPatadir = false;
	movPikaPata = false;

	angulo = 180.0f;
	radioA = 0.0f;
	betha = 0.0f;
	angOffset = 1.0f;
	dir = true;
	
	bool* keys;
	numCam = 0;
	pj = false;
	movPika = false;
	
	CreateObjects();
	CreateShaders();
	CreateAvatar();

	ISoundEngine* engine = createIrrKlangDevice();
	ISoundEngine* engine2 = createIrrKlangDevice();
	ISoundEngine* engine3 = createIrrKlangDevice();
	ISoundEngine* ataque = createIrrKlangDevice();

	if (!engine)
		return 0; // error starting up the engine
	if (!engine2)
		return 0; // error starting up the engine
	if (!engine3)
		return 0; // error starting up the engine
	if (!ataque)
		return 0; // error starting up the engine

	ataque->play3D("Music/dragonite.mp3", vec3df(0.0f, 0.0f, 0.0f), true, false, true);

	ataque->setSoundVolume(0.0);
	engine->setSoundVolume(0.0f);
	engine2->setSoundVolume(0.0f);
	engine3->setSoundVolume(1.0f);


	ISound* music= engine->play3D("Music/Ruta1poke.mp3",
		vec3df(0.0f, 0.0f, 0.0f), true, false, true);

	ISound* music2 = engine2->play3D("Music/LoL2.mp3",
		vec3df(0.0f, 0.0f, 0.0f), true, false, true);

	ISound* music3 = engine3->play3D("Music/Deltarune.mp3",
		vec3df(0.0f, 0.0f, 0.0f), true, false, true);

	/*------------------
	 CREACION DE CAMARAS
	-------------------*/

	camera[0] = Camera(glm::vec3(0.0f, 1500.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 10.0f, 0.5f, pj);
	camera[1] = Camera(glm::vec3(-300.0f, 270.0f, -200.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, -1.0f, 10.0f, 0.5f, pj);
	camera[2] = Camera(glm::vec3(0.0f, 1500.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 60.0f, -100.0f, 10.0f, 0.5f, pj);

	/*------------------
	  CARGA DE TEXTURAS
	--------------------*/
	trainerTexture = Texture("Textures/TrainerBueno.tga");
	trainerTexture.LoadTextureA();
	
	/*----------------
	  CARGA DE MODELOS
	------------------*/
	PikaCuerpo = Model();
	PikaCuerpo.LoadModel("Models/Pokemon/PikachuBody.obj");
	PikaPata = Model();
	PikaPata.LoadModel("Models/Pokemon/PikachuLeg.obj");
	DragoniteB = Model();
	DragoniteB.LoadModel("Models/Pokemon/DragoniteSinNada.obj");
	DragoniteA = Model();
	DragoniteA.LoadModel("Models/Pokemon/DragoniteBrazo.obj");
	DragoniteW1 = Model();
	DragoniteW1.LoadModel("Models/Pokemon/DragoniteAla.obj");
	DragoniteW2 = Model();
	DragoniteW2.LoadModel("Models/Pokemon/DragoniteAla2.obj");
	CentroP = Model();
	CentroP.LoadModel("Models/Pokemon/CentroPoke.obj");
	Escenario = Model();
	Escenario.LoadModel("Models/EscenarioCompleto.obj");
	PoroCuerpo = Model();
	PoroCuerpo.LoadModel("Models/LOL/poroCuerpo.obj");
	PoroPata = Model();
	PoroPata.LoadModel("Models/LOL/poroPata.obj");
	Arbol = Model();
	Arbol.LoadModel("Models/arbol.obj");
	Freljord = Model();
	Freljord.LoadModel("Models/Freljord.obj");
	Portal = Model();
	Portal.LoadModel("Models/Portal.obj");
	BattleStage = Model();
	BattleStage.LoadModel("Models/Pokemon/BattleStage.obj");
	Letrero1 = Model();
	Letrero1.LoadModel("Models/Pokemon/LetreroMadera.obj");
	Letrero2 = Model();
	Letrero2.LoadModel("Models/Pokemon/LetreroPLastico.obj");
	CasaPokemon = Model();
	CasaPokemon.LoadModel("Models/Pokemon/CasaPoke.obj");
	Humo = Model();
	Humo.LoadModel("Models/nubes.obj");
	Antorcha = Model();
	Antorcha.LoadModel("Models/LOL/antorcha.obj");
	Poste = Model();
	Poste.LoadModel("Models/Pokemon/LamparaPoke.obj");
	Pokeball = Model();
	Pokeball.LoadModel("Models/Pokemon/Pokeball.obj");
	Bulbasaur = Model();
	Bulbasaur.LoadModel("Models/Pokemon/Bulbasaur.obj");
	Squirtle = Model();
	Squirtle.LoadModel("Models/Pokemon/Squirtle.obj");
	Charmander = Model();
	Charmander.LoadModel("Models/Pokemon/charmander.obj");
	slowpoke = Model();
	slowpoke.LoadModel("Models/Pokemon/slowth.obj");
	Pino = Model();
	Pino.LoadModel("Models/LOL/pino.obj");
	Cabania = Model();
	Cabania.LoadModel("Models/LOL/cabania.obj");
	Espada = Model();
	Espada.LoadModel("Models/LOL/espada.obj");
	Hacha = Model();
	Hacha.LoadModel("Models/LOL/hacha.obj");
	Arco = Model();
	Arco.LoadModel("Models/LOL/arco.obj");
	Flecha = Model();
	Flecha.LoadModel("Models/LOL/flecha.obj");


	/*--------------------
	  CARGADO DE SKYBOXES
	----------------------*/

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/right-day.tga");
	skyboxFaces.push_back("Textures/Skybox/left-day.tga");
	skyboxFaces.push_back("Textures/Skybox/bottom-day.tga");
	skyboxFaces.push_back("Textures/Skybox/top-day.tga");
	skyboxFaces.push_back("Textures/Skybox/front-day.tga");
	skyboxFaces.push_back("Textures/Skybox/back-day.tga");
	skybox = Skybox(skyboxFaces);

	std::vector<std::string> skyboxFaces2;
	skyboxFaces2.push_back("Textures/Skybox/right-sunset.tga");
	skyboxFaces2.push_back("Textures/Skybox/left-sunset.tga");
	skyboxFaces2.push_back("Textures/Skybox/bottom-sunset.tga");
	skyboxFaces2.push_back("Textures/Skybox/top-sunset.tga");
	skyboxFaces2.push_back("Textures/Skybox/front-sunset.tga");
	skyboxFaces2.push_back("Textures/Skybox/back-sunset.tga");
	skybox2 = Skybox(skyboxFaces2);


	std::vector<std::string> skyboxFaces3;
	skyboxFaces3.push_back("Textures/Skybox/right-night.tga");
	skyboxFaces3.push_back("Textures/Skybox/left-night.tga");
	skyboxFaces3.push_back("Textures/Skybox/bottom-night.tga");
	skyboxFaces3.push_back("Textures/Skybox/top-night.tga");
	skyboxFaces3.push_back("Textures/Skybox/front-night.tga");
	skyboxFaces3.push_back("Textures/Skybox/back-night.tga");
	skybox3 = Skybox(skyboxFaces3);


	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//contador de luces
	unsigned int pointLightCount = 0, tmp = MAX_POINT_LIGHTS;
	unsigned int spotLightCount = 0, tmp2 = 1;

	//Luces
	red = 1.0f;
	green = 1.0f;
	blue = 1.0f;
	
	//Arbol
	pointLights[0] = PointLight(0.0f, 0.627f, 0.89f,
		10.5f, 10.5f,
		0.0f, 800.0f, 0.0,
		0.1f, 0.1f, 0.0f);
	pointLightCount++;
	
	//Contador para las luces temporales
	pointLightCount++;

	pointLightCount++;
	
	//luz que simula las llamas del Dragonite
	spotLights[0] = SpotLight(1.0f, 0.5f, 0.16f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 5000.0f);

	periodoDN = 1.0f;
	Ciclo = true;
	SkyboxSelector = 1;

	/*---------------------------------
	  ANIMACION POR KEYFRAME DEL PORO
	-----------------------------------*/

	KeyFrame[0].movPoro_x = 0.0f;
	KeyFrame[0].movPoro_y = 0.0f;
	KeyFrame[0].movPoro_z = 0.0f;
	KeyFrame[0].rotPoro = 0;

	KeyFrame[1].movPoro_x = 0.0f;
	KeyFrame[1].movPoro_y = 0.0f;
	KeyFrame[1].movPoro_z = 0.0f;
	KeyFrame[1].rotPoro = 45.0f;

	KeyFrame[2].movPoro_x = -50.0f;
	KeyFrame[2].movPoro_y = 0.0f;
	KeyFrame[2].movPoro_z = -50.0f;
	KeyFrame[2].rotPoro = 45.0f;

	KeyFrame[3].movPoro_x = -100.0f;
	KeyFrame[3].movPoro_y = 0.0f;
	KeyFrame[3].movPoro_z = -100.0f;
	KeyFrame[3].rotPoro = 45.0f;

	KeyFrame[4].movPoro_x = -150.0f;
	KeyFrame[4].movPoro_y = 0.0f;
	KeyFrame[4].movPoro_z = -150.0f;
	KeyFrame[4].rotPoro = 45.0f;

	KeyFrame[5].movPoro_x = -200.0f;
	KeyFrame[5].movPoro_y = 0.0f;
	KeyFrame[5].movPoro_z = -200.0f;
	KeyFrame[5].rotPoro = 45.0f;

	KeyFrame[6].movPoro_x = -250.0f;
	KeyFrame[6].movPoro_y = 0.0f;
	KeyFrame[6].movPoro_z = -250.0f;
	KeyFrame[6].rotPoro = 45.0f;

	KeyFrame[7].movPoro_x = -300.0f;
	KeyFrame[7].movPoro_y = 0.0f;
	KeyFrame[7].movPoro_z = -300.0f;
	KeyFrame[7].rotPoro = 45.0f;

	KeyFrame[8].movPoro_x = -350.0f;
	KeyFrame[8].movPoro_y = 0.0f;
	KeyFrame[8].movPoro_z = -350.0f;
	KeyFrame[8].rotPoro = 45.0f;

	KeyFrame[9].movPoro_x = -400.0f;
	KeyFrame[9].movPoro_y = 0.0f;
	KeyFrame[9].movPoro_z = -400.0f;
	KeyFrame[9].rotPoro = 45.0f;

	KeyFrame[10].movPoro_x = -450.0f;
	KeyFrame[10].movPoro_y = 0.0f;
	KeyFrame[10].movPoro_z = -450.0f;
	KeyFrame[10].rotPoro = 45.0f;

	KeyFrame[11].movPoro_x = -450.0f;
	KeyFrame[11].movPoro_y = 0.0f;
	KeyFrame[11].movPoro_z = -450.0f;
	KeyFrame[11].rotPoro = 135.0f;

	KeyFrame[12].movPoro_x = -500.0f;
	KeyFrame[12].movPoro_y = 0.0f;
	KeyFrame[12].movPoro_z = -400.0f;
	KeyFrame[12].rotPoro = 135.0f;

	KeyFrame[13].movPoro_x = -550.0f;
	KeyFrame[13].movPoro_y = 0.0f;
	KeyFrame[13].movPoro_z = -350.0f;
	KeyFrame[13].rotPoro = 135.0f;

	KeyFrame[14].movPoro_x = -600.0f;
	KeyFrame[14].movPoro_y = 0.0f;
	KeyFrame[14].movPoro_z = -300.0f;
	KeyFrame[14].rotPoro = 135.0f;

	KeyFrame[15].movPoro_x = -650.0f;
	KeyFrame[15].movPoro_y = 0.0f;
	KeyFrame[15].movPoro_z = -250.0f;
	KeyFrame[15].rotPoro = 135.0f;

	KeyFrame[15].movPoro_x = -700.0f;
	KeyFrame[15].movPoro_y = 0.0f;
	KeyFrame[15].movPoro_z = -200.0f;
	KeyFrame[15].rotPoro = 135.0f;

	KeyFrame[16].movPoro_x = -750.0f;
	KeyFrame[16].movPoro_y = 0.0f;
	KeyFrame[16].movPoro_z = -150.0f;
	KeyFrame[16].rotPoro = 135.0f;

	KeyFrame[17].movPoro_x = -800.0f;
	KeyFrame[17].movPoro_y = 0.0f;
	KeyFrame[17].movPoro_z = -100.0f;
	KeyFrame[17].rotPoro = 135.0f;

	KeyFrame[18].movPoro_x = -850.0f;
	KeyFrame[18].movPoro_y = 0.0f;
	KeyFrame[18].movPoro_z = -50.0f;
	KeyFrame[18].rotPoro = 135.0f;

	KeyFrame[19].movPoro_x = -900.0f;
	KeyFrame[19].movPoro_y = 0.0f;
	KeyFrame[19].movPoro_z = 0.0f;
	KeyFrame[19].rotPoro = 135.0f;

	KeyFrame[20].movPoro_x = -900.0f;
	KeyFrame[20].movPoro_y = 0.0f;
	KeyFrame[20].movPoro_z = 0.0f;
	KeyFrame[20].rotPoro = 90.0f;

	KeyFrame[21].movPoro_x = -950.0f;
	KeyFrame[21].movPoro_y = 0.0f;
	KeyFrame[21].movPoro_z = 0.0f;
	KeyFrame[21].rotPoro = 90.0f;

	KeyFrame[22].movPoro_x = -1000.0f;
	KeyFrame[22].movPoro_y = 0.0f;
	KeyFrame[22].movPoro_z = 0.0f;
	KeyFrame[22].rotPoro = 90.0f;

	KeyFrame[23].movPoro_x = -1050.0f;
	KeyFrame[23].movPoro_y = 0.0f;
	KeyFrame[23].movPoro_z = 0.0f;
	KeyFrame[23].rotPoro = 90.0f;

	KeyFrame[24].movPoro_x = -1100.0f;
	KeyFrame[24].movPoro_y = 0.0f;
	KeyFrame[24].movPoro_z = 0.0f;
	KeyFrame[24].rotPoro = 90.0f;

	KeyFrame[25].movPoro_x = -1100.0f;
	KeyFrame[25].movPoro_y = 100.0f;
	KeyFrame[25].movPoro_z = 0.0f;
	KeyFrame[25].rotPoro = 90.0f;

	KeyFrame[26].movPoro_x = -1100.0f;
	KeyFrame[26].movPoro_y = 0.0f;
	KeyFrame[26].movPoro_z = 0.0f;
	KeyFrame[26].rotPoro = 90.0f;

	KeyFrame[27].movPoro_x = -1100.0f;
	KeyFrame[27].movPoro_y = 100.0f;
	KeyFrame[27].movPoro_z = 0.0f;
	KeyFrame[27].rotPoro = 90.0f;

	KeyFrame[28].movPoro_x = -1100.0f;
	KeyFrame[28].movPoro_y = 0.0f;
	KeyFrame[28].movPoro_z = 0.0f;
	KeyFrame[28].rotPoro = 90.0f;

	KeyFrame[29].movPoro_x = -1100.0f;
	KeyFrame[29].movPoro_y = 0.0f;
	KeyFrame[29].movPoro_z = 0.0f;
	KeyFrame[29].rotPoro = 270.0f;

	KeyFrame[30].movPoro_x = -1050.0f;
	KeyFrame[30].movPoro_y = 0.0f;
	KeyFrame[30].movPoro_z = 0.0f;
	KeyFrame[30].rotPoro = 270.0f;

	KeyFrame[31].movPoro_x = -1000.0f;
	KeyFrame[31].movPoro_y = 0.0f;
	KeyFrame[31].movPoro_z = 0.0f;
	KeyFrame[31].rotPoro = 270.0f;

	KeyFrame[32].movPoro_x = -950.0f;
	KeyFrame[32].movPoro_y = 0.0f;
	KeyFrame[32].movPoro_z = 0.0f;
	KeyFrame[32].rotPoro = 270.0f;

	KeyFrame[33].movPoro_x = -900.0f;
	KeyFrame[33].movPoro_y = 0.0f;
	KeyFrame[33].movPoro_z = 0.0f;
	KeyFrame[33].rotPoro = 270.0f;

	KeyFrame[34].movPoro_x = -850.0f;
	KeyFrame[34].movPoro_y = 0.0f;
	KeyFrame[34].movPoro_z = 0.0f;
	KeyFrame[34].rotPoro = 270.0f;

	KeyFrame[35].movPoro_x = -800.0f;
	KeyFrame[35].movPoro_y = 100.0f;
	KeyFrame[35].movPoro_z = 0.0f;
	KeyFrame[35].rotPoro = 270.0f;

	KeyFrame[36].movPoro_x = -750.0f;
	KeyFrame[36].movPoro_y = 0.0f;
	KeyFrame[36].movPoro_z = 0.0f;
	KeyFrame[36].rotPoro = 270.0f;

	KeyFrame[37].movPoro_x = -700.0f;
	KeyFrame[37].movPoro_y = 100.0f;
	KeyFrame[37].movPoro_z = 0.0f;
	KeyFrame[37].rotPoro = 270.0f;

	KeyFrame[38].movPoro_x = -650.0f;
	KeyFrame[38].movPoro_y = 0.0f;
	KeyFrame[38].movPoro_z = 0.0f;
	KeyFrame[38].rotPoro = 270.0f;

	KeyFrame[39].movPoro_x = -600.0f;
	KeyFrame[39].movPoro_y = 0.0f;
	KeyFrame[39].movPoro_z = 0.0f;
	KeyFrame[39].rotPoro = 270.0f;

	KeyFrame[40].movPoro_x = -550.0f;
	KeyFrame[40].movPoro_y = 0.0f;
	KeyFrame[40].movPoro_z = 0.0f;
	KeyFrame[40].rotPoro = 270.0f;

	KeyFrame[41].movPoro_x = -500.0f;
	KeyFrame[41].movPoro_y = 0.0f;
	KeyFrame[41].movPoro_z = 0.0f;
	KeyFrame[41].rotPoro = 270.0f;

	KeyFrame[42].movPoro_x = -450.0f;
	KeyFrame[42].movPoro_y = 0.0f;
	KeyFrame[42].movPoro_z = 0.0f;
	KeyFrame[42].rotPoro = 270.0f;

	KeyFrame[43].movPoro_x = -400.0f;
	KeyFrame[43].movPoro_y = 0.0f;
	KeyFrame[43].movPoro_z = 0.0f;
	KeyFrame[43].rotPoro = 270.0f;

	KeyFrame[44].movPoro_x = -350.0f;
	KeyFrame[44].movPoro_y = 0.0f;
	KeyFrame[44].movPoro_z = 0.0f;
	KeyFrame[44].rotPoro = 270.0f;

	KeyFrame[45].movPoro_x = -300.0f;
	KeyFrame[45].movPoro_y = 100.0f;
	KeyFrame[45].movPoro_z = 0.0f;
	KeyFrame[45].rotPoro = 270.0f;

	KeyFrame[46].movPoro_x = -250.0f;
	KeyFrame[46].movPoro_y = 0.0f;
	KeyFrame[46].movPoro_z = 0.0f;
	KeyFrame[46].rotPoro = 270.0f;

	KeyFrame[47].movPoro_x = -200.0f;
	KeyFrame[47].movPoro_y = 100.0f;
	KeyFrame[47].movPoro_z = 0.0f;
	KeyFrame[47].rotPoro = 270.0f;

	KeyFrame[48].movPoro_x = -150.0f;
	KeyFrame[48].movPoro_y = 0.0f;
	KeyFrame[48].movPoro_z = 0.0f;
	KeyFrame[48].rotPoro = 270.0f;

	KeyFrame[49].movPoro_x = -100.0f;
	KeyFrame[49].movPoro_y = 0.0f;
	KeyFrame[49].movPoro_z = 0.0f;
	KeyFrame[49].rotPoro = 270.0f;

	KeyFrame[50].movPoro_x = -50.0f;
	KeyFrame[50].movPoro_y = 0.0f;
	KeyFrame[50].movPoro_z = 0.0f;
	KeyFrame[50].rotPoro = 270.0f;

	KeyFrame[51].movPoro_x = -0.0f;
	KeyFrame[51].movPoro_y = 0.0f;
	KeyFrame[51].movPoro_z = 0.0f;
	KeyFrame[51].rotPoro = 270.0f;

	KeyFrame[52].movPoro_x = 0.0f;
	KeyFrame[52].movPoro_y = 0.0f;
	KeyFrame[52].movPoro_z = 0.0f;
	KeyFrame[52].rotPoro = 0.0f;

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		/*------------------------
		  MUSICA SEGUN EL SEGMENTO
		-------------------------*/

		glm::vec3 mediaselector;
		mediaselector = camera[numCam].getCameraPosition();
		if (mediaselector.x < -250.0f) {
			engine3->setSoundVolume(0.1f);
			engine2->setSoundVolume(0.2f);
			engine->setSoundVolume(1.0f);
			
		}
		else if (mediaselector.x > 250.0f) {
			engine3->setSoundVolume(0.1f);
			engine2->setSoundVolume(0.7f);
			engine->setSoundVolume(0.1f);
		}
		else {
			engine3->setSoundVolume(1.0f);
			engine2->setSoundVolume(0.1f);
			engine->setSoundVolume(0.1f);
		}
		/*----------------------------
				SHOW DE LUCES
		-----------------------------*/

		//Aleatorizador de luces para show
		srand(time(NULL));
		red = (rand() % 100) / 100.0f;
		green = (rand() % 100) / 100.0f;
		blue = (rand() % 100) / 100.0f;

		//Portal pokemon
		pointLights[1] = PointLight(red, green, blue,
			1.5f, 5.0f,
			-1055.0f, 360.0f, -1060.0f,
			0.1f, 0.1f, 0.0f);

		//Portal freljord
		pointLights[2] = PointLight(red, green, blue,
			1.5f, 5.0f,
			1050.0f, 280.0f, 1055.0f,
			0.1f, 0.1f, 0.0f);

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*-----------------
		   CICLO DIA-NOCHE 
		-------------------*/

		//Ciclo Dia-Noche con repeticion infinita (O hasta que se desborde periodoDN)
		if (glfwGetTime() >= periodoDN) {
			valorCiclo -= 0.01f;
			periodoDN += 1.0f;
		}
		if (valorCiclo < 0.0f) {
			Ciclo = false;
		}
		if (!Ciclo) {
			valorCiclo += 0.001;
			if (valorCiclo > 0.7) {
				Ciclo = true;
			}
		}

		//luz direccional
		mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
			valorCiclo, valorCiclo,
			0.0f, 0.0f, -1.0f);

		/*------------------
		   SKYBOX DINAMICO
		--------------------*/

		if (valorCiclo < 0.47f and valorCiclo > 0.24) {
			SkyboxSelector = 2;

		}
		else if (valorCiclo < 0.24f and valorCiclo > -0.1) {
			SkyboxSelector = 3;
		}
		else
			SkyboxSelector = 1;

		switch (SkyboxSelector)
		{
		case 1:
			skybox.DrawSkybox(camera[numCam].calculateViewMatrix(), projection);
			luz = false;
			break;
		case 2:
			skybox2.DrawSkybox(camera[numCam].calculateViewMatrix(), projection);
			luz = false;
			break;
		case 3:
			skybox3.DrawSkybox(camera[numCam].calculateViewMatrix(), projection);
			luz = true;
			break;
		default:
			break;
		}

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		numCam = mainWindow.camNum();
		if (numCam == 1) {
			pj = true;
		}
		else{
			pj = false;
		}
		
		//Recibir eventos del usuario
		glfwPollEvents();
		camera[numCam].keyControl(mainWindow.getsKeys(), deltaTime);
		if (numCam == 2) {
			camera[numCam].mouseControl(mainWindow.getXChange(), 0.0f, pj);
		}
		else {
			camera[numCam].mouseControl(mainWindow.getXChange(), mainWindow.getYChange(), pj);
		}
		
		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera[numCam].calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera[numCam].getCameraPosition().x, camera[numCam].getCameraPosition().y, camera[numCam].getCameraPosition().z);

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		
		//Control de ataque
		vec3df pos(centrox, 1000.0f + 100.0f * sin(angulo * toRadians), centroz);
		if (mainWindow.flama() == true) {
			ataque->setSoundVolume(0.5);
			spotLightCount = tmp2;
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else if (mainWindow.flama() == false)
		{
			ataque->setSoundVolume(0.0);
			spotLightCount = tmp2 - 1;
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}

		//Direccion de luz
		glm::vec3 dirDragonite = glm::vec3(glm::vec3(-glm::cos(radioA * toRadians + 46.0f), -0.5f, glm::sin(radioA * toRadians + 46.0f)));
		spotLights[0].SetFlash(glm::vec3(centrox, 100.0f * sin(angulo * toRadians) + 1010.0f, centroz), dirDragonite);

		if (luz) {
			pointLights[3] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				-200.0f, 220.0f, 1000.0f,
				0.1f, 0.1f, 0.0f);
			pointLights[4] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				400.0f, 220.0f, 1200.0f,
				0.1f, 0.1f, 0.0f);
			pointLights[5] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				1200.0f, 220.0f, 400.0f,
				0.1f, 0.1f, 0.0f);
			pointLights[6] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				1200.0f, 220.0f, -200.0f,
				0.1f, 0.1f, 0.0f);
			pointLights[7] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				550.0f, 220.0f, 550.0f,
				0.1f, 0.1f, 0.0f);
			pointLights[8] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				-1050.0f, 420.0f, 1000.0f,
				0.1f, 0.1f, 0.0f);
			pointLights[9] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				900.0f, 420.0f, -1050.0f,
				0.1f, 0.1f, 0.0f);
			pointLights[10] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				-945.0f, 420.0f, -490.0f,
				0.1f, 0.1f, 0.0f);
			pointLights[11] = PointLight(1.0f, 0.5f, 0.16f,
				1.5f, 5.0f,
				10.0f, 420.0f, -820.0f,
				0.1f, 0.1f, 0.0f);
			
			pointLightCount = tmp;
			shaderList[0].SetPointLights(pointLights, pointLightCount);

		}else {
			pointLightCount = tmp - 9;
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		
		/*----------------------
		  ANIMACIONES CICLICAS
		------------------------*/
		
		//Pikachu
		if (giroPika > 360.0f) {
			movPikax = 0.0f;
			movPikaz = 0.0f;
			giroPika = 0.0f;
		}

		if (movPika) {
			if (movPikax > -1050.0f and movPikaz <= 0.0f) {
				movPikax -= movOffset * deltaTime;
				if (movPikax < -1015.0f && giroPika < 90.0f) {
					giroPika += 5.0 * deltaTime;
				}
			}
			if (movPikaz < 1050.0f and movPikax <= -1050.0f) {
				movPikaz += movOffset * deltaTime;
				if (movPikaz > 1010.0f && giroPika < 225.0f) {
					giroPika += 5.0 * deltaTime;
				}
			}
			if (movPikaz > 0.0f and movPikax < 0.0f and giroPika >= 225.0f) {
				movPikax += movOffset * deltaTime;
				movPikaz -= movOffset * deltaTime;
				if (movPikaz < 40.0f and movPikax > -40.0f and giroPika < 360.0f) {
					giroPika += 5.0 * deltaTime;
				}
			}
		}

		if (movPikaPata) {
			if (pikaPatadir) {
				pikaPataD += 3.0 * deltaTime;
				pikaPataI -= 3.0 * deltaTime;
			}
			if (pikaPataD > 35.0f) {
				pikaPatadir = false;
			}
			if (!pikaPatadir) {
				pikaPataI += 3.0 * deltaTime;
				pikaPataD -= 3.0 * deltaTime;
			}
			if (pikaPataD < -35.0f) {
				pikaPatadir = true;
			}
		}

		//Dragonite
		if (betha < 5000 && dir) {
			betha += angOffset * deltaTime;
		}
		else {
			dir = false;
		}
		if (betha > 0 && !dir) {
			betha -= angOffset * deltaTime;
		}
		else {
			dir = true;
		}
		radioA = 1.0f + 0.2 * betha;
		centrox = radioA * glm::cos(betha * toRadians);
		centroz = radioA * glm::sin(betha * toRadians);
		angulo += 1.0f * deltaTime;
		if (angulo > 359.0f) {
			angulo = 0.0f;
		}

		if (dirAlas) {
			rotAlaD += 1.0 * deltaTime;
			rotAlaI -= 1.0 * deltaTime;
		}
		if (rotAlaD > 50.0f) {
			dirAlas = false;
		}
		if (!dirAlas) {
			rotAlaD -= 1.0 * deltaTime;
			rotAlaI += 1.0 * deltaTime;
		}
		if (rotAlaD < 0.0f) {
			dirAlas = true;
		}
		
		//Poro
		if (movPoroPata) {
			if (poroPatadir) {
				poroPataD += 0.3 * deltaTime;
				poroPataI -= 0.3 * deltaTime;
			}
			if (poroPataD > 2.0f) {
				poroPatadir = false;
			}
			if (!poroPatadir) {
				poroPataI += 0.3 * deltaTime;
				poroPataD -= 0.3 * deltaTime;
			}
			if (poroPataD < -2.0f) {
				poroPatadir = true;
			}
		}

		//Humo de la cabaña
		movNubes += 0.05f * deltaTime;
		if (movNubes > 5.0f) {
			movNubes = 0.0f;
		}

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		/*-----------------
				AVATAR
		-------------------*/
		
		//Body
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(camera[1].getCameraPosition().x + 200.0f, camera[1].getCameraPosition().y - 90.0f, camera[1].getCameraPosition().z));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotAvatar * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		trainerTexture.UseTexture();
		meshList[6]->RenderMesh();

		//leg1
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		model = glm::rotate(model, rotExtremidades * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -12.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		//Leg2
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		model = glm::rotate(model, -rotExtremidades * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(4.0f, -12.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		trainerTexture.UseTexture();
		meshList[5]->RenderMesh();

		//Arm1
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.0f, 10.0f, -2.0f));
		model = glm::rotate(model, rotExtremidades * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		trainerTexture.UseTexture();
		meshList[7]->RenderMesh();

		//Arm2
		model = modelaux;
		model = glm::translate(model, glm::vec3(8.0f, 10.0f, -2.0f));
		model = glm::rotate(model, -rotExtremidades * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		trainerTexture.UseTexture();
		meshList[8]->RenderMesh();

		//Head
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 12.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));;
		trainerTexture.UseTexture();
		meshList[9]->RenderMesh();
		
		/*-----------------
		OBJETOS COMPARTIDOS
		-------------------*/

		//Escenario completo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -100.0f, 0.0f));
		model = glm::scale(model, glm::vec3(200.0f, 200.0f, 200.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Escenario.RenderModel();

		//Portal pokemon
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1075.0f, 360.0f, -1080.0f));
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Portal.RenderModel();

		//Portal freljord
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1080.0f, 280.0f, 1085.0f));
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Portal.RenderModel();

		//Arbol Hub Central
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 65.0f, 0.0f));
		model = glm::scale(model, glm::vec3(250.0f, 300.0f, 250.0f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		/*------------------
		  UNIVERSO POKEMON
		--------------------*/

		//Lamparas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1050.0f, 420.0f, 1000.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(900.0f, 420.0f, -1050.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-945.0f, 420.0f, -490.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 420.0f, -820.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Poste.RenderModel();

		//Letrero Madera
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-580.0f, 185.0f, 60.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Letrero1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-910.0f, 185.0f, 60.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Letrero1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-580.0f, 185.0f, 510.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Letrero1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-910.0f, 185.0f, 510.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Letrero1.RenderModel();

		//Casas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-980.0f, 235.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaPokemon.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-650.0f, 235.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaPokemon.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-980.0f, 235.0f, 450.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaPokemon.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-650.0f, 235.0f, 450.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CasaPokemon.RenderModel();

		//Letrero Plastico
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-500.0f, 190.0f, -250.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Letrero2.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(710.0f, 190.0f, -850.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Letrero2.RenderModel();

		//Centro Pokemon
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(600.0f, 225.0f, -1000.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CentroP.RenderModel();

		//Pikachu
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(250.0f, 180.0f, -700.0f));
		model = glm::translate(model, glm::vec3(movPikax, movPikay, movPikaz));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, giroPika * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		aux = model;
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PikaCuerpo.RenderModel();

		//Pikachu patas
		model = aux;
		model = glm::translate(model, glm::vec3(17.5f, -30.0f, -17.5f));
		model = glm::rotate(model, pikaPataD * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PikaPata.RenderModel();
		model = aux;
		model = glm::translate(model, glm::vec3(-17.5f, -30.0f, -17.5f));
		model = glm::rotate(model, pikaPataD * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PikaPata.RenderModel();
		model = aux;
		model = glm::translate(model, glm::vec3(-17.5f, -30.0f, 9.0f));
		model = glm::rotate(model, pikaPataI * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PikaPata.RenderModel();
		model = aux;
		model = glm::translate(model, glm::vec3(17.5f, -30.0f, 9.0f));
		model = glm::rotate(model, pikaPataI * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PikaPata.RenderModel();

		//Escenario Pelea
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-450.0f, 165.0f, -700.0f));
		model = glm::scale(model, glm::vec3(70.0f, 10.0f, 70.0f));
		model = glm::rotate(model, 135.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BattleStage.RenderModel();

		//Dragonite

		//Cuerpo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1000.0f, 0.0f));
		model = glm::translate(model, glm::vec3(centrox, 100.0f * sin(angulo * toRadians), centroz));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -radioA * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		aux = model;
		model = glm::rotate(model, 45.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DragoniteB.RenderModel();

		//Alas
		model = aux;
		model = glm::translate(model, glm::vec3(-7.0f, 20.0f, 0.0f));
		model = glm::rotate(model, rotAlaD * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DragoniteW1.RenderModel();
		model = aux;
		model = glm::translate(model, glm::vec3(7.0f, 20.0f, 0.0f));
		model = glm::rotate(model, rotAlaI * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DragoniteW2.RenderModel();

		//Brazos
		model = aux;
		model = glm::translate(model, glm::vec3(-12.0f, 7.0f, 20.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DragoniteA.RenderModel();
		model = aux;
		model = glm::translate(model, glm::vec3(12.0f, 7.0f, 20.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DragoniteA.RenderModel();

		//Charmander
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-250.0f, 190.0f, -750.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, 135.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Charmander.RenderModel();

		//Bulbasaur
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 180.0f, -750.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, 135.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bulbasaur.RenderModel();

		//Squirtle
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-350.0f, 190.0f, -750.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, 135.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Squirtle.RenderModel();

		//Slowpoke
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-350.0f, 180.0f, -700.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		model = glm::rotate(model, -45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		slowpoke.RenderModel();

		//Pokebola para animacion de evolucion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(300.0f, 260.0f, -1000.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pokeball.RenderModel();

		/*-----------
		 UNIVERSO LOL
		-------------*/

		//Poro
		//Cuerpo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1050.0f, 185.0f, 1050.0f));
		model = glm::translate(model, glm::vec3(movPoro_x, movPoro_y, movPoro_z));
		model = glm::rotate(model, rotPoro * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		movPJ(mainWindow.getsKeys());
		aux = model;
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PoroCuerpo.RenderModel();
		//Patas
		model = aux;
		model = glm::translate(model, glm::vec3(7.5f, -20.0f, 7.5f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, poroPataD));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PoroPata.RenderModel();
		model = aux;
		model = glm::translate(model, glm::vec3(7.5f, -20.0f, -7.5f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, poroPataD));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PoroPata.RenderModel();
		model = aux;
		model = glm::translate(model, glm::vec3(-7.5f, -20.0f, 7.5f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, poroPataI));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PoroPata.RenderModel();
		model = aux;
		model = glm::translate(model, glm::vec3(-7.5f, -20.0f, -7.5f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, poroPataI));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PoroPata.RenderModel();

		//Cabaña
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(-5.0f, 2.5f, 18.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cabania.RenderModel();

		
		//Humo
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::translate(model, glm::vec3(-40.0f, 36.0f, 107.0f));
		model = glm::translate(model, glm::vec3(0.0f, movNubes, 0.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Humo.RenderModel();

		//Antorcha
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 160.0f, 1000.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Antorcha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(400.0f, 160.0f, 1200.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Antorcha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1200.0f, 160.0f, 400.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Antorcha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1200.0f, 160.0f, -200.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Antorcha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(550.0f, 160.0f, 550.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Antorcha.RenderModel();

		//Pino
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(-5.0f, 2.5f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(18.0f, 2.5f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(5.0f, 2.5f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(10.0f, 2.5f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(3.0f, 2.5f, 14.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(18.0f, 2.5f, 12.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(10.0f, 2.5f, 14.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(3.0f, 2.5f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(-11.0f, 2.5f, 14.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(14.0f, 2.5f, -11.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();


		//Espada
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(20.0f, 6.4f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Espada.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(30.0f, 6.9f, 20.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Espada.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(20.0f, 6.4f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Espada.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(50.0f, 6.4f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Espada.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(15.0f, 6.4f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Espada.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
		model = glm::translate(model, glm::vec3(15.0f, 2.5f, -5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pino.RenderModel();

		//Hacha
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(40.0f, 6.4f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Hacha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(40.0f, 6.4f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Hacha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(10.0f, 6.4f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Hacha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(15.0f, 6.4f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Hacha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(35.0f, 6.4f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Hacha.RenderModel();

		//Arco
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(35.0f, 6.4f, 35.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arco.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(5.0f, 6.4f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arco.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(10.0f, 6.4f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arco.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(25.0f, 6.4f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arco.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(5.0f, 6.4f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arco.RenderModel();

		//Flecha
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(35.0f, 6.9f, -30.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(40.0f, 6.9f, -35.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(40.0f, 6.9f, -20.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(50.0f, 6.9f, -30.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(30.0f, 6.9f, -20.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(-30.0f, 6.9f, 30.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(-5.0f, 6.9f, 15.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(-15.0f, 6.9f, 20.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(-25.0f, 6.9f, 50.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(-20.0f, 6.9f, 25.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::translate(model, glm::vec3(-15.0f, 6.9f, 30.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	music->drop();
	engine->drop();
	music2->drop();
	engine2->drop();
	music3->drop();
	engine3->drop();
	return 0;
}

void movPJ(bool* keys) {
	if (pj) {
		if (keys[GLFW_KEY_A])
		{
			if (prueba) {
				rotExtremidades += 3.0f * deltaTime;
				if (rotExtremidades > 45.0f) {
					prueba = false;
				}
			}
			else {
				rotExtremidades -= 3.0f * deltaTime;
				if (rotExtremidades < -45.0f) {
					prueba = true;
				}
			}

			if ((rotAvatar >= -1.0f and rotAvatar <= 90.0f) or (rotAvatar >= 270.0f and rotAvatar <= 360.0f))
				rotAvatar += 1.0f * deltaTime;
			else if ((rotAvatar >= 90.0f and rotAvatar <= 180.0f) or (rotAvatar >= 180.0f and rotAvatar <= 270.0f))
				rotAvatar -= 1.0f * deltaTime;
		}
		if (keys[GLFW_KEY_S])
		{
			if (prueba) {
				rotExtremidades += 3.0f * deltaTime;
				if (rotExtremidades > 45.0f) {
					prueba = false;
				}
			}
			else {
				rotExtremidades -= 3.0f * deltaTime;
				if (rotExtremidades < -45.0f) {
					prueba = true;
				}
			}

			if ((rotAvatar >= 90.0f and rotAvatar <= 180.0f) or (rotAvatar >= -1.0f and rotAvatar <= 90.0f))
				rotAvatar += 1.0f * deltaTime;
			else if ((rotAvatar >= 180.0f and rotAvatar <= 270.0f) or (rotAvatar >= 270.0f and rotAvatar <= 360.0f))
				rotAvatar -= 1.0f * deltaTime;
		}
		if (keys[GLFW_KEY_D])
		{
			if (prueba) {
				rotExtremidades += 3.0f * deltaTime;
				if (rotExtremidades > 45.0f) {
					prueba = false;
				}
			}
			else {
				rotExtremidades -= 3.0f * deltaTime;
				if (rotExtremidades < -45.0f) {
					prueba = true;
				}
			}

			if ((rotAvatar >= 180.0f and rotAvatar <= 270.0f) or (rotAvatar >= 90.0f and rotAvatar <= 180.0f))
				rotAvatar += 1.0f * deltaTime;
			else if ((rotAvatar >= 0.0f and rotAvatar <= 90.0f) or (rotAvatar >= 270.0f and rotAvatar <= 360.0f)) {
				rotAvatar -= 1.0f * deltaTime;
				if (rotAvatar < 0.0f)
					rotAvatar += 360.0f;
			}
		}
		if (keys[GLFW_KEY_W])
		{
			if (prueba) {
				rotExtremidades += 3.0f * deltaTime;
				if (rotExtremidades > 45.0f) {
					prueba = false;
				}
			}
			else {
				rotExtremidades -= 3.0f * deltaTime;
				if (rotExtremidades < -45.0f) {
					prueba = true;
				}
			}

			if ((rotAvatar >= 270.0f and rotAvatar <= 360.0f) or (rotAvatar >= 180.0f and rotAvatar <= 270.0f))
				rotAvatar += 1.0f * deltaTime;
			else if ((rotAvatar >= 0.0f and rotAvatar <= 90.0f) or (rotAvatar >= 90.0f and rotAvatar <= 180.0f))
				rotAvatar -= 1.0f * deltaTime;
		}
		if (rotAvatar > 360.0f) {
			rotAvatar = 0.0f;
		}
	}
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;
				movPoroPata = true;
			}
			else
			{
				play = false;
				movPoroPata = false;
			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}

	if (keys[GLFW_KEY_Z])
	{
		if (ciclo < 1)
		{
			if (movPika)
			{
				movPika = false;
				movPikaPata = false;
			}
			else if (!movPika) {
				movPika = true;
				movPikaPata = true;
			}
			ciclo++;
			ciclo2 = 0;
		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movPoro_x es: %f\n", movPoro_x);
			printf("movPoro_y es: %f\n", movPoro_y);
			printf("movPoro_z es: %f\n", movPoro_z);
			printf("rotPoro es: %f\n", rotPoro);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			movPoro_x += 50.0f;
			printf("movPoro_x es: %f\n", movPoro_x);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_3])
	{
		if (ciclo < 1)
		{
			movPoro_x -= 50.0f;
			printf("movPoro_x es: %f\n", movPoro_x);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_4])
	{
		if (ciclo < 1)
		{
			movPoro_y += 50.0f;
			printf("movPoro_y es: %f\n", movPoro_y);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_5])
	{
		if (ciclo < 1)
		{
			movPoro_y -= 50.0f;
			printf("movPoro_y es: %f\n", movPoro_y);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_8])
	{
		if (ciclo < 1)
		{
			movPoro_z -= 50.0f;
			printf("movPoro_z es: %f\n", movPoro_z);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_9])
	{
		if (ciclo < 1)
		{
			printf("movPoro_z es: %f\n", movPoro_z);
			movPoro_z += 50.0f;
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_6])
	{
		if (ciclo < 1)
		{
			rotPoro += 15.0f;
			printf("rotPoro es: %f\n", rotPoro);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_7])
	{
		if (ciclo < 1)
		{
			rotPoro -= 15.0f;
			printf("rotPoro es: %f\n", rotPoro);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
		}
	}
}