HALITE_HPP = Halite/Component.hpp
HALITE_CPP = Halite/Constants Halite/Resistor Halite/Halite
LIBHALITE  = libhalite

HPP = $(HALITE_HPP)
CPP = $(HALITE_CPP)

CXX = c++
CFLAGS = -I$(shell pwd) -I/usr/include/eigen3 -std=c++2a

PLUGIN = SpiceLV2
BUNDLE = $(PLUGIN).lv2

$(LIBHALITE).so: $(LIBHALITE).a
	gcc -shared -o $(LIBHALITE).so $(LIBHALITE).a

$(LIBHALITE).a: $(addsuffix .o, $(HALITE_CPP))
	ar rcs $@ $^

$(addsuffix .o, $(CPP)): %.o: %.cpp $(HPP)
	$(CXX) -c $(CFLAGS) $< -o $@

test: $(LIBHALITE).a
	$(CXX) $(CFLAGS) Test/Test.cpp $(LIBHALITE).a -o Test/Test

SpiceLV2/SpiceLV2.so: $(LIBHALITE).a SpiceLV2/SpiceLV2.cpp
	g++ $(CFLAGS) -shared -fPIC -DPIC $(LIBHALITE).a SpiceLV2/SpiceLV2.cpp `pkg-config --cflags --libs lv2-plugin` -o SpiceLV2/SpiceLV2.so

bundle: SpiceLV2/manifest.ttl SpiceLV2/SpiceLV2.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp SpiceLV2/manifest.ttl SpiceLV2/SpiceLV2.so $(BUNDLE)

clean:
	rm -rf $(LIBHALITE).a $(LIBHALITE).so $(addsuffix .o, $(CPP)) Test/Test $(BUNDLE) SpiceLV2/SpiceLV2.so