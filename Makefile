objects = HIR.o HIRModel.o HIRRecord.o HIRConfig.o HIRArgument.o \
HIRTrain.o HIRTest.o

HIR_objects = HIR.o HIRModel.o HIRRecord.o HIRConfig.o HIRArgument.o

HIR : $(objects)
	g++ -o HIRTest $(HIR_objects) HIRTest.o -std=c++11
	g++ -o HIRTrain $(HIR_objects) HIRTrain.o -std=c++11

HIRTest : $(HIR_objects) HIRTest.o
	g++ -o HIRTest $(HIR_objects) HIRTest.o -std=c++11

HIRTrain : $(HIR_objects) HIRTrain.o
	g++ -o HIRTrain $(HIR_objects) HIRTrain.o -std=c++11

HIR.o : HIR.cpp HIR.h
	g++ -c HIR.cpp -std=c++11

HIRArgument.o : HIRArgument.cpp HIR.h
	g++ -c HIRArgument.cpp -std=c++11

HIRConfig.o : HIRConfig.cpp HIRConfig.h
	g++ -c HIRConfig.cpp -std=c++11

HIRModel.o : HIRModel.cpp HIRModel.h
	g++ -c HIRModel.cpp -std=c++11

HIRRecord.o : HIRRecord.cpp HIRRecord.h
	g++ -c HIRRecord.cpp -std=c++11

HIRTrain.o : HIRTrain.cpp HIR.h
	g++ -c HIRTrain.cpp -std=c++11

HIRTest.o : HIRTest.cpp HIR.h
	g++ -c HIRTest.cpp -std=c++11

clean :
	-rm HIRTrain HIRtest $(objects)
