#include "Mesh.h"


Mesh::Mesh(vector<Vertice> vertices, vector<unsigned int> indices, vector<Textura> texturas)
{
	// passa os atributos
	this->vertices = vertices;
	this->indices = indices;
	this->texturas = texturas;

	// chama a construção
	MeshGL();

}

void Mesh::Render(Shader shader)
{
	// numero para nomenclatura
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	// para cada textura carregada:
	for (unsigned int i = 0; i < texturas.size(); i++)
	{
		// ativa a textura atual
		glActiveTexture(GL_TEXTURE0 + i);

		string numero;
		string nome = texturas[i].tipo;

		// adiciona o número ao nome
		if (nome == "texture_diffuse")
			numero = std::to_string(diffuseNr++);

		else if (nome == "texture_specular")
			numero = std::to_string(specularNr++);

		// usa o shader para renderizar a textura
		shader.setFloat(("material." + nome + numero).c_str(), i);

		// bind com a textura atual
		glBindTexture(GL_TEXTURE_2D, texturas[i].id);
	}

	// ativa a textura
	glActiveTexture(GL_TEXTURE0);

	// faz o bind no arrayy de vertices
	glBindVertexArray(VAO);

	// desenha os vertices
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// desfaz o bind
	glBindVertexArray(0);
}

void Mesh::MeshGL()
{

	// cria buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// carrega as informações
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertice), &vertices[0], GL_STATIC_DRAW);

	// organiza em índices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// posições
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)0);

	// normais
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, normal));

	// textura
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, uv));

	// desfaz o bind
	glBindVertexArray(0);
}
