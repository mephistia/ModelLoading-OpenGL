#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <Mesh.h>

class Model
{
public:
	
	// construtor
	Model(string caminho);

	void Render(Shader shader);

private:

	vector<Textura> texturasCarregadas;

	vector<Mesh> meshes;

	// criar com assimp
	Mesh CriarMesh(aiMesh *mesh, const aiScene *cena);

	// criar com assimp
	void CriarNodo(aiNode *nodo, const aiScene *cena);

	//carregar com assimp
	vector<Textura> CarregarTextura(aiMaterial *mat, aiTextureType tipo, string nomeTipo);

	string caminho;

	unsigned int CarregarTexturaImagem(const char *caminho, const string &pasta, bool gama = false);

};

#endif