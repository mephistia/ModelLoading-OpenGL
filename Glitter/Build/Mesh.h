#pragma once
#ifndef MESH_H
#define MESH_H



#include <Shader.h>

#include <vector>

using namespace std;

// struct com dados
struct Vertice {
	glm::vec3 posicao;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Textura {
	unsigned int id;
	string tipo;
	string caminho;
};


class Mesh
{

public:
	// construtor
	Mesh(vector<Vertice> vertices, vector<unsigned int> indices, vector<Textura> texturas);

	// desenhar
	void Render(Shader shader);

	// atributos
	vector<Vertice> vertices;
	vector<unsigned int> indices;
	vector<Textura> texturas;

private:
	
	unsigned int VAO, VBO, EBO;
	
	// constroi em OpenGL
	void MeshGL();
};

#endif