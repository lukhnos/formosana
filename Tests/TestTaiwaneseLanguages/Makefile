CXXFLAGS += -I../../ExternalLibraries/OpenVanilla-Part/Headers -I../../Headers/TaiwaneseRomanization

DEPS = ../../Source/TaiwaneseRomanization/TaiwaneseRomanization.o ../../Source/TaiwaneseRomanization/VowelHelper.o 

hakkasyllist: hakkasyllist.o $(DEPS)
	$(CXX) -o $@ $^

syllist: syllist.o $(DEPS)
	$(CXX) -o $@ $^

clean:
	rm -f $(DEPS) syllist hakkkasyllist

