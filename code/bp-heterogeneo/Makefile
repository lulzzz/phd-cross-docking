OPT = main.o
GRAFO_OBJ = Grafo.o
ROTAS_OBJ = geraRotas.o Rota.o
CPLEX_OBJ = ModeloCplex.o ModeloHeuristica.o
ARVORE_OBJ = NoArvore.o NoArvoreArcos.o ListaNosAtivos.o
SUBPROBLEMA_OBJ = Elementar.o StateSpaceRelax.o
ALL_OBJS = $(CPLEX_OBJ) $(ROTAS_OBJ) $(GRAFO_OBJ) $(SUBPROBLEMA_OBJ) $(ARVORE_OBJ) $(OPT)

SYSTEM     = x86_sles10_4.1
LIBFORMAT  = static_pic
CPLEXDIR      = /usr/ilog/cplex
CONCERTDIR    = /usr/ilog/concert
CCC = g++
CCOPT = -O -fPIC -fexceptions -DNDEBUG -DIL_STD
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

geraCols: $(ALL_OBJS)
	$(CCC) -Wall -W -g -DNOTE $(ALL_OBJS) -o geraCols.e $(CCLNFLAGS)

Grafo.o: Grafo.cpp
	$(CCC) $(CCFLAGS) -c -g Grafo.cpp

ModeloCplex.o: ModeloCplex.cpp
	$(CCC) $(CCFLAGS) -c -g ModeloCplex.cpp

geraRotas.o: geraRotas.cpp
	$(CCC) $(CCFLAGS) -c -g geraRotas.cpp

Rota.o: Rota.cpp
	$(CCC) $(CCFLAGS) -c -g Rota.cpp

Elementar.o: Elementar.cpp
	$(CCC) $(CCFLAGS) -c -g Elementar.cpp

StateSpaceRelax.o: StateSpaceRelax.cpp
	$(CCC) $(CCFLAGS) -c -g StateSpaceRelax.cpp

NoArvore.o: NoArvore.cpp
	$(CCC) $(CCFLAGS) -c -g NoArvore.cpp

NoArvoreArcos.o: NoArvoreArcos.cpp
	$(CCC) $(CCFLAGS) -c -g NoArvoreArcos.cpp

ListaNosAtivos.o: ListaNosAtivos.cpp
	$(CCC) $(CCFLAGS) -c -g ListaNosAtivos.cpp

ModeloHeuristica.o: ModeloHeuristica.cpp
	$(CCC) $(CCFLAGS) -c -g ModeloHeuristica.cpp
 
main.o: main.cpp
	$(CCC) $(CCFLAGS) -c -g main.cpp

clean:
	rm *.o -f
	rm *~ -f
	rm geraCols.e -f
