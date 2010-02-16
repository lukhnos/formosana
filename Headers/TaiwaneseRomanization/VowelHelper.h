//
// VowelHelper.h
//
// Copyright (c) 2006-2010 Lukhnos D. Liu (http://lukhnos.org)
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

//
// This module follows the POJ/TL convention of tone notation.
// The tone of any DT input must be re-mapped before feeding into
// the symbolForVowel() function
//
// TL does not compose nn and ou (TL uses oo--there's no ou), 
// use the flag "composePOJOnlySymbols=false" when composing TL vowels.
//
// If you are feeding the ending N (in POJ) to this function, you
// must explicity covert it into "nn" (lower case only). This is because
// "n" can mean other things elsewhere.

#ifndef VowelHelper_h
#define VowelHelper_h

#include <string>
#include <map>
#include "OVUTF8Helper.h"

namespace Formosa {
    namespace TaiwaneseRomanization {

        using namespace std;
        
        struct VowelDecompositionState {
            VowelDecompositionState();
            string representedSymbol;
            unsigned int representedTone;
            map<char, VowelDecompositionState> transitions;
        };
        
        class VowelHelper {
        public:
            static const string symbolForVowel(const string& vowel, unsigned int tone, bool usePOJStyleOUAndNN, bool usePOJStyleNinthToneMark = false, bool composeII = false);

            static char diacriticShorthandFromTone(unsigned int tone);
            static unsigned int toneFromDiacriticShorthand(char shorthand);
            static bool isDiacriticShorthand(char shorthand);
            static const string uppercasedVowelFromVowel(const string& vowel);

            static const string queryFormFromComposedForm(const string& composed);            
            static VowelDecompositionState& vowelDecompositionGroundState();
        private:
            static map<string, map<unsigned int, string> >* c_vowelTable;
            static map<string, string>* c_toUppercasedVowelTable;
            static VowelDecompositionState *c_groundState;
            
            static void populateVowelTables();
            
            static void addStringToState(const string& s, const string &repSymbol, unsigned int repTone, VowelDecompositionState& startState);
            static void populateDecompositionStateMachine();
        };
    };
};

#endif
