#include "ModeloHeuristica.h"

ModeloHeuristica::~ModeloHeuristica(){
	int maxVeiculos = lambdaInt.getSize();
	objective.end();
	constraints1.endElements();
	constraints2.endElements();
	constraints3.endElements();
	constraints4.endElements();
	constraints5.endElements();
	
	for (int k = 0; k < maxVeiculos; ++k)
	{
		lambdaInt[k].endElements();
		gammaInt[k].endElements();
		for (int kC = 0; kC < maxVeiculos; ++kC)
		{
			tauInt[k][kC].endElements();
		}
		tauInt[k].end();
	}
	lambdaInt.end();
	gammaInt.end();
	tauInt.end();

	cplex.end();
	model.end();
}

ModeloHeuristica::ModeloHeuristica(ModeloCplex& mCplex){
	model = IloModel(mCplex.env);
	cplex = IloCplex(model);
	cplex.setOut(mCplex.env.getNullStream());

	initVars(mCplex);
	setObjectiveFunction(mCplex);
	setConstraints1e2(mCplex);
	setConstraints3e4(mCplex);
	setConstraints5(mCplex);
}

void ModeloHeuristica::initVars(ModeloCplex& mCplex){
	int totalRotas = 0;
	int totalBasicas = 0;
	int maxV = mCplex.maxVeic;
	int numCommod = mCplex.nCommodities;

	for (int k = 0; k < maxV; ++k){
		totalRotas += mCplex.qRotasForn[k];
		totalRotas += mCplex.qRotasCons[k];
	}

	//Caso o problema tenha um numero de rotas suficiente para colocar todas, faz isto
	if (totalRotas < 8000){
		for (int k = 0; k < maxV; ++k){
			for (int r = 0; r < mCplex.qRotasForn[k]; ++r){
				mCplex.ptrRotasForn[k][r]->setBasica();
			}
			for (int r = 0; r < mCplex.qRotasCons[k]; ++r){
				mCplex.ptrRotasCons[k][r]->setBasica();
			}
		}
	}else{
		totalBasicas = 0;
		for (int k = 0; k < maxV; ++k){
			for (int r = 0; r < mCplex.qRotasForn[k]; ++r){
				if (mCplex.ptrRotasForn[k][r]->getBasica()) ++totalBasicas;
			}
			for (int r = 0; r < mCplex.qRotasCons[k]; ++r){
				if (mCplex.ptrRotasCons[k][r]->getBasica()) ++totalBasicas;
			}
		}

		int aleat, k = 0;
		while(totalBasicas < 8000){
			aleat = rand() % mCplex.qRotasForn[k];
			mCplex.ptrRotasForn[k][aleat]->setBasica();
			totalBasicas += mCplex.ptrRotasForn[k][aleat]->getNumApontadores();

			aleat = rand() % mCplex.qRotasCons[k];
			mCplex.ptrRotasCons[k][aleat]->setBasica();
			totalBasicas += mCplex.ptrRotasCons[k][aleat]->getNumApontadores();

			if (++k == maxV) k = 0;
		}
	}

	lambdaInt = IloArray<IloIntVarArray>(mCplex.env, maxV);
	for (int i = 0; i < maxV; i++){
		totalBasicas = 0;
		for (int r = 0; r < mCplex.qRotasForn[i]; ++r){
			if (mCplex.ptrRotasForn[i][r]->getBasica()) ++totalBasicas;
		}
		lambdaInt[i] = IloIntVarArray(mCplex.env, totalBasicas, 0, 1);
	}

	gammaInt = IloArray<IloIntVarArray>(mCplex.env, maxV);
	for (int i = 0; i < maxV; i++){
		totalBasicas = 0;
		for (int r = 0; r < mCplex.qRotasCons[i]; ++r){
			if (mCplex.ptrRotasCons[i][r]->getBasica()) ++totalBasicas;
		}
		gammaInt[i] = IloIntVarArray(mCplex.env, totalBasicas, 0, 1);
	}

	tauInt = IloArray < IloArray < IloIntVarArray > > (mCplex.env, maxV);
	for (int kForn = 0; kForn < maxV; kForn++)
	{
		tauInt[kForn] = IloArray < IloIntVarArray > (mCplex.env, maxV);
		for (int kCons = 0; kCons < maxV; kCons++)
		{
			tauInt[kForn][kCons] = IloIntVarArray(mCplex.env, (numCommod+1), 0, 1);
		}
	}
}

void ModeloHeuristica::setObjectiveFunction(ModeloCplex& mCplex){
	int count, maxV = mCplex.maxVeic;
	int numCommod = mCplex.nCommodities;
	IloExpr expObj = IloExpr(mCplex.env);

	for (int k = 0; k < maxV; ++k){
		count = -1;
		for (int r = 0; r < mCplex.qRotasForn[k]; ++r){
			if (mCplex.ptrRotasForn[k][r]->getBasica()) expObj += (mCplex.ptrRotasForn[k][r]->getCusto() * lambdaInt[k][++count]);
		}
	}
	for (int k = 0; k < maxV; ++k){
		count = -1;
		for (int r = 0; r < mCplex.qRotasCons[k]; ++r){
			if (mCplex.ptrRotasCons[k][r]->getBasica())	expObj += (mCplex.ptrRotasCons[k][r]->getCusto() * gammaInt[k][++count]);
		}
	}
	
	for (int kForn = 0; kForn < maxV; kForn++)
	{
		for (int kCons = 0; kCons < maxV; kCons++)
		{
			expObj += MAIS_INFINITO * tauInt[kForn][kCons][0];
			for (int i = 1; i <= numCommod; i++)
			{
				if (kForn != kCons) expObj += mCplex.custoTrocaCD * tauInt[kForn][kCons][i];
			}
		}
	}

	objective = IloObjective(mCplex.env, expObj);
	model.add(objective);
}

void ModeloHeuristica::setConstraints1e2(ModeloCplex& mCplex){
	int count, maxV = mCplex.maxVeic;
	constraints1 = IloRangeArray(mCplex.env, maxV);
	constraints2 = IloRangeArray(mCplex.env, maxV);

	for (int k = 0; k < maxV; ++k){
		count = -1;
		IloExpr exp1 = IloExpr(mCplex.env);
		for (int r = 0; r < mCplex.qRotasForn[k]; ++r){
			if (mCplex.ptrRotasForn[k][r]->getBasica()) exp1 += lambdaInt[k][++count];
		}
		constraints1[k] = (exp1 == 1);
		model.add(constraints1[k]);
		exp1.end();

		count =-1;
		IloExpr exp2 = IloExpr(mCplex.env);
		for (int r = 0; r < mCplex.qRotasCons[k]; ++r){
			if (mCplex.ptrRotasCons[k][r]->getBasica()) exp2 += gammaInt[k][++count];
		}
		constraints2[k] = (exp2 == 1);
		model.add(constraints2[k]);
		exp2.end();
	}
}

void ModeloHeuristica::setConstraints3e4(ModeloCplex& mCplex){
	int count, maxV = mCplex.maxVeic;
	int numCommod = mCplex.nCommodities;
	vector<short int*>* matrizA_qr = mCplex.A_qr;
	vector<short int*>* matrizB_qr = mCplex.B_qr;
	constraints3 = IloRangeArray(mCplex.env, numCommod);
	constraints4 = IloRangeArray(mCplex.env, numCommod);

	for (int i = 1; i <= numCommod; ++i){
		IloExpr exp3 = IloExpr(mCplex.env);
		for (int k = 0; k < maxV; ++k){
			count = -1;
			for (int r = 0; r < mCplex.qRotasForn[k]; ++r){
				if (mCplex.ptrRotasForn[k][r]->getBasica()){
					++count;
					if(matrizA_qr[k][r][i] > 0){
						exp3 += lambdaInt[k][count];
					}
				}
			}
		}
		constraints3[i-1] = (exp3 == 1);
		model.add(constraints3[i-1]);

		IloExpr exp4 = IloExpr(mCplex.env);
		for (int k = 0; k < maxV; ++k){
			count = -1;
			for (int r = 0; r < mCplex.qRotasCons[k]; ++r){
				if (mCplex.ptrRotasCons[k][r]->getBasica()){
					++count;
					if (matrizB_qr[k][r][i] > 0){
						exp4 += gammaInt[k][count];
					}
				}
			}
		}
		constraints4[i-1] = (exp4 == 1);
		model.add(constraints4[i-1]);
	}
}

void ModeloHeuristica::setConstraints5(ModeloCplex& mCplex){
	int contadorRestr = 0;
	int count, maxV = mCplex.maxVeic;
	int numCommod = mCplex.nCommodities;
	vector<short int*>* matrizA_qr = mCplex.A_qr;
	vector<short int*>* matrizB_qr = mCplex.B_qr;
	constraints5 = IloRangeArray(mCplex.env, maxV*maxV*numCommod);
	
	for (int kForn = 0; kForn < maxV; ++kForn)
	{
		for (int kCons = 0; kCons < maxV; ++kCons)
		{
			for (int i = 1; i <= numCommod; ++i)
			{
				IloExpr exp5 = IloExpr(mCplex.env);
	
				count = -1;
				for (int r = 0; r < mCplex.qRotasForn[kForn]; ++r)
				{
					if (mCplex.ptrRotasForn[kForn][r]->getBasica())
					{
						++count;
						if (matrizA_qr[kForn][r][i] > 0)
						{
							exp5 += lambdaInt[kForn][count];
						}
					}
				}
				
				count = -1;
				for (int r = 0; r < mCplex.qRotasCons[kCons]; ++r)
				{
					if (mCplex.ptrRotasCons[kCons][r]->getBasica())
					{
						++count;
						if (matrizB_qr[kCons][r][i] > 0)
						{
							exp5 += gammaInt[kCons][count];
						}
					}
				}	
				
				exp5 -= 2*tauInt[kForn][kCons][i];
				constraints5[contadorRestr] = (exp5 <= 1);
				model.add(constraints5[contadorRestr]);
				++contadorRestr;
				exp5.end();
			}
		}
	}
}

float ModeloHeuristica::optimize(){
	int limite = 10 * lambdaInt.getSize() + pow(3, tauInt[0].getSize()/10);
	cplex.setParam(IloCplex::TiLim, limite);
	cplex.solve();
	if ((cplex.getStatus() == IloAlgorithm::Feasible) || (cplex.getStatus() == IloAlgorithm::Optimal)){
		return cplex.getObjValue();
	}else{
		return MAIS_INFINITO;
	}
}
