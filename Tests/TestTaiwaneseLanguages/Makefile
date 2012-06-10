CXXFLAGS += -I../../ExternalLibraries/OpenVanilla-Part/Headers -I../../Headers/TaiwaneseRomanization

INFILES = syllist.o ../../Source/TaiwaneseRomanization/TaiwaneseRomanization.o ../../Source/TaiwaneseRomanization/VowelHelper.o 

syllist: $(INFILES)
	$(CXX) -o $@ $^

clean:
	rm -f $(INFILES) syllist
