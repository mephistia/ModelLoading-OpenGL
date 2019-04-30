#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC
#define STBI_REALLOC
#define STBI_FREE
#include <stb_image.h>



Model::Model(string caminho)
{

	Assimp::Importer import;

	// importa e triangula
	const aiScene *cena = import.ReadFile(caminho, aiProcess_Triangulate | aiProcess_FlipUVs);

	// modifica o atributo caminho
	this->caminho = caminho.substr(0, caminho.find_last_of('/'));

	// cria o nodo
	CriarNodo(cena->mRootNode, cena);
}



void Model::Render(Shader shader)
{
	// chama a função Render de cada Mesh
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Render(shader);
}



Mesh Model::CriarMesh(aiMesh * mesh, const aiScene * cena)
{
	// vetores para armazenar os dados
	vector<Vertice> vertices;
	vector<unsigned int> indices;
	vector<Textura> texturas;

	// para cada vértice da mesh
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertice vertice;

		// vetor placeholder
		glm::vec3 vetorAssimp; 

		// posicoes da mesh
		vetorAssimp.x = mesh->mVertices[i].x;
		vetorAssimp.y = mesh->mVertices[i].y;
		vetorAssimp.z = mesh->mVertices[i].z;
		vertice.posicao = vetorAssimp;

		// normais
		vetorAssimp.x = mesh->mNormals[i].x;
		vetorAssimp.y = mesh->mNormals[i].y;
		vetorAssimp.z = mesh->mNormals[i].z;

		// passa o vetor
		vertice.normal = vetorAssimp;


		// se tem textura
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vetorTextura;
			
			// passa as coordenadas
			vetorTextura.x = mesh->mTextureCoords[0][i].x;
			vetorTextura.y = mesh->mTextureCoords[0][i].y;

			// passa a textura para o vertice
			vertice.uv = vetorTextura;
		}

		// se nao
		else
			// deixa vazio
			vertice.uv = glm::vec2(0.0f, 0.0f); 

		// por fim adiciona na lista
		vertices.push_back(vertice);
	}

	// faces da mesh
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		// passa os índices de faces para o vetor de índices
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}


	// materiais
	aiMaterial* material = cena->mMaterials[mesh->mMaterialIndex];

	// cria vetor de textura difusa
	vector<Textura> diffuseMaps = CarregarTextura(material, aiTextureType_DIFFUSE, "texture_diffuse");
	texturas.insert(texturas.end(), diffuseMaps.begin(), diffuseMaps.end());

	// cria vetor de textura especular
	vector<Textura> specularMaps = CarregarTextura(material, aiTextureType_SPECULAR, "texture_specular");
	texturas.insert(texturas.end(), specularMaps.begin(), specularMaps.end());


	// retorna a mesh com os dados carregados
	return Mesh(vertices, indices, texturas);
}



void Model::CriarNodo(aiNode * nodo, const aiScene * cena)
{
	// cria todas as meshes do nodo
	for (unsigned int i = 0; i < nodo->mNumMeshes; i++)
	{
		aiMesh *mesh = cena->mMeshes[nodo->mMeshes[i]];
		meshes.push_back(CriarMesh(mesh, cena));
	}

	// repete para os nodos filhos (recursivo)
	for (unsigned int i = 0; i < nodo->mNumChildren; i++)
	{
		CriarNodo(nodo->mChildren[i], cena);
	}
}



vector<Textura> Model::CarregarTextura(aiMaterial * mat, aiTextureType tipo, string nomeTipo)
{

	// cria vetor de texturas
	vector<Textura> texturas;

	// para cada tipo de textura
	for (unsigned int i = 0; i < mat->GetTextureCount(tipo); i++)
	{
		aiString str;
		mat->GetTexture(tipo, i, &str);

	
		bool existe = false;

		// para cada textura
		for (unsigned int j = 0; j < texturasCarregadas.size(); j++)
		{
			// compara se o caminho é o mesmo de algum
			if (std::strcmp(texturasCarregadas[j].caminho.data(), str.C_Str()) == 0)
			{
				texturas.push_back(texturasCarregadas[j]);

				// se for, ela já existe e não precisa ser carregada
				existe = true;
				break;
			}
		}

		// se não existe a textura
		if (!existe)
		{  
			Textura textura;
			textura.id = CarregarTexturaImagem(str.C_Str(), this->caminho);
			textura.tipo = nomeTipo;
			textura.caminho = str.C_Str();
			texturas.push_back(textura);
			texturas.push_back(textura);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return texturas;
}

unsigned int Model::CarregarTexturaImagem(const char * caminho, const string & pasta, bool gama)
{
	// passar o caminho como nome do arquivo
	string nomeArquivo = string(caminho);
	nomeArquivo = pasta + '/' + nomeArquivo;

	// gerar texturas
	unsigned int IDTextura;
	glGenTextures(1, &IDTextura);

	int largura, altura, nComponentes;
	unsigned char *info = stbi_load(nomeArquivo.c_str(), &largura, &altura, &nComponentes, 0);
	if (info)
	{

		// especifica o formato do arquivo
		GLenum formato;
		if (nComponentes == 1)
			formato = GL_RED;
		else if (nComponentes == 3)
			formato = GL_RGB;
		else if (nComponentes == 4)
			formato = GL_RGBA;

		// bind da textura
		glBindTexture(GL_TEXTURE_2D, IDTextura);

		// cria a textura
		glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0, formato, GL_UNSIGNED_BYTE, info);

		// gera mipmap
		glGenerateMipmap(GL_TEXTURE_2D);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		// libera memória
		stbi_image_free(info);
	}
	else
	{
		std::cout << "Falha no carregamento da textura no caminho: " << caminho << std::endl;
		stbi_image_free(info);
	}

	return IDTextura;
}

