#include "geraRotas.h"
using namespace std;

Rota** geraPoolRotas(Grafo* g, bool camForn, int numVeic){
	bool inseriuVertice;
	int it, k, v, cargaTotal;
	int nCommodities = g->getNumCmdt();
	int *capacidade = new int[numVeic];
	for ( int c = 0; c < numVeic; ++c ) capacidade[c] = g->getCapacVeiculo( c );
	
	int depositoArtificial = 2*nCommodities+1;
	int ajuste = (camForn) ? 0 : nCommodities;
	char grupo = (camForn) ? 'F' : 'C';
	int numVerticesDescobertos;
	ptrNoListaEnc* rotas = new ptrNoListaEnc[numVeic];
	int* vertices = new int[nCommodities+1];

	for (it = 0; it < 1000000; ++it){
		//Inicializa os valores do vetor de vertices a serem cobertos para a iteracao
		numVerticesDescobertos = nCommodities;
		for (int i = 1; i <= nCommodities; ++i){
			vertices[i] = i;
		}

		//A principio, cria uma rota com um unico vertice (escolhido aleatoriamente) para cada veiculo
		for (k = 0; k < numVeic; ++k){
			do
			{
				v = (rand() % numVerticesDescobertos) + 1;
			}
			while( g->getPesoCommodity( vertices[v] + ajuste ) > capacidade[k] );

			rotas[k] = new NoListaEnc();
			rotas[k]->vertice = vertices[v];
			rotas[k]->prox = NULL;

			//Substitue o vertice coberto pelo ultimo do vetor
			vertices[v] = vertices[numVerticesDescobertos];
			--numVerticesDescobertos;
		}

		//Partindo-se das rotas singulares acima, tenta-se inserir aleatoriamente os vertices nas rotas ate cobrir todos
		k = 0;
		ptrNoListaEnc tmp;
		while ( numVerticesDescobertos > 0 )
		{
			inseriuVertice = false;
			v = (rand() % numVerticesDescobertos) + 1; //escolhe-se o indice de um vertice (vertices[v] representa o vertice)

			//Tenta inserir o vertice escolhido em um veiculo, passando para o proximo, caso nao seja possivel inserir no atual
			for ( int veic = k; veic < (k+numVeic); ++veic )
			{
				//verifica se a cargaTotal excede a capacidade do veiculo
				cargaTotal = g->getPesoCommodity(vertices[v]+ajuste);
				tmp = rotas[veic % numVeic];
				while(tmp != NULL){
					cargaTotal += g->getPesoCommodity(tmp->vertice+ajuste);
					tmp = tmp->prox;
				}

				if ( cargaTotal <= capacidade[veic % numVeic] )
				{
					//insere sempre o vertice entre 0 e rota[k]. Esta estrategia pode influenciar no custo da solucao, mas nao na sua viabilidade
					tmp = new NoListaEnc();
					tmp->vertice = vertices[v];
					tmp->prox = rotas[veic % numVeic];
					rotas[veic % numVeic] = tmp;

					vertices[v] = vertices[numVerticesDescobertos];
					k = (veic % numVeic) + 1;
					--numVerticesDescobertos;
					inseriuVertice = true;
					break;
				}
			}

			if (!inseriuVertice){
				//Essa tentativa nao deu certo, reinicia os vetores e ponteiros e passa para a proxima tentativa
				break;
			}
		}

		if (numVerticesDescobertos == 0){ //Significa que obteve as rotas viaveis
			break;
		}else{ //Limpa as rotas sem sucesso obtidas, para iniciar uma nova iteracao procurando por outras rotas
			for (int k = 0; k < numVeic; ++k){
				tmp = rotas[k];
				while(tmp != NULL){
					delete tmp;
					tmp = tmp->prox;
				}
			}

		}
	}

	if ( numVerticesDescobertos == 0 )
	{
		ptrNoListaEnc aux;
		Rota** poolRotas = new Rota*[numVeic];

		//Insere as rotas associadas a solucao viavel encontrada
		for ( int k = 0; k < numVeic; ++k )
		{
			poolRotas[k] = new Rota();
			while ( rotas[k] != NULL )
			{
				poolRotas[k]->inserirVerticeFim(rotas[k]->vertice+ajuste);
				aux = rotas[k];
				rotas[k] = rotas[k]->prox;
				delete aux;
			}
			poolRotas[k]->inserirVerticeInicio(0);
			poolRotas[k]->inserirVerticeFim(depositoArtificial);
			poolRotas[k]->setCusto(g, k);
		}
		delete capacidade;
		return poolRotas;
	}
	else
	{
		cout << "Não encontrou rotas viaveis para k = " << numVeic << " - " << grupo << endl;
		exit(0);
	}
}
