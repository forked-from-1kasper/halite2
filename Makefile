HALITE_HPP = Halite/Component.hpp Halite/Interface.hpp Halite/Eigen.hpp

HALITE_CPP  = Halite/Constants Halite/ExportVoltage
HALITE_CPP += Halite/Voltage Halite/Capacitor Halite/Resistor Halite/Probe Halite/Function
HALITE_CPP += Halite/JunctionPN Halite/Diode Halite/BJT
HALITE_CPP += Halite/MNACell Halite/MNASystem Halite/TransientSimulation Halite/Halite

LIBHALITE  = libhalite

HPP = $(HALITE_HPP)
CPP = $(HALITE_CPP)

CXX = c++
CFLAGS  = -O3 -fPIC -I$(shell pwd) -I/usr/include/eigen3 -std=c++2a
CFLAGS += -DVERBOSE

PLUGIN = SpiceLV2
BUNDLE = $(PLUGIN).lv2

$(LIBHALITE).so: $(LIBHALITE).a
	gcc -shared -o $(LIBHALITE).so $(LIBHALITE).a

$(LIBHALITE).a: $(addsuffix .o, $(HALITE_CPP))
	ar rcs $@ $^

$(addsuffix .o, $(CPP)): %.o: %.cpp %.hpp $(HPP)
	$(CXX) -c $(CFLAGS) $< -o $@

test: $(LIBHALITE).a
	$(CXX) $(CFLAGS) Test/Test.cpp $(LIBHALITE).a -o Test/Test

SpiceLV2/SpiceLV2.so: $(LIBHALITE).a SpiceLV2/SpiceLV2.cpp
	g++ $(CFLAGS) -shared -DPIC SpiceLV2/SpiceLV2.cpp $(LIBHALITE).a `pkg-config --cflags --libs lv2-plugin` -o SpiceLV2/SpiceLV2.so

bundle: SpiceLV2/manifest.ttl SpiceLV2/SpiceLV2.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp SpiceLV2/manifest.ttl SpiceLV2/SpiceLV2.so $(BUNDLE)

clean:
	rm -rf $(LIBHALITE).a $(LIBHALITE).so $(addsuffix .o, $(CPP)) Test/Test $(BUNDLE) SpiceLV2/SpiceLV2.so
