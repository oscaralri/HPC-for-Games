#pragma once
#include <vector>
#include <Model.h>

struct ModelHandle
{
	ModelHandle() {}
	ModelHandle(int i, int c) : Index(i), Counter(c) {}

};

class ResourceManager
{
	// voy a hacerlo solo para Model -> cuando funcione lo hago generico o veo que hago

	// la idea principal:
		// los recursos guardan el handle (indice para acceder al array con los recursos + magic number que comprueba que lugar memoria bien)
		// aprovechar espacios vacíos de memoria comprobnado con counter (magic number)

public:



private:
	std::vector<Model> models;
	
	// cómo traduzco esto a que haya diferentes tipos
		// quizás se podría llegar a hacer algo con ECS
			// así cada uno tiene los entities sobre los que tiene que recorrer
			// un sistema genérico que simplemente recorre esos entities 
	//vector<Textures>
	//vector<Models> 

	// MI PROBLEMA REALMENTE AHORA MISMO ES HACERLO GENÉRICO
		// LA LÓGICA DEL HANDLE COMO TAL NO SERÁ MUCHO MÁS COMPLEJA QUE LO QUE HE COPIADO DEL VIDEO

};

