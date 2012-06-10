#include "TaiwaneseRomanization.h"
#include "OVStringHelper.h"
#include <iostream>

using namespace std;
using namespace OpenVanilla;
using namespace Formosa::TaiwaneseRomanization;

int main()
{
    while (!cin.eof()) {
        string line;
        getline(cin, line);
        
        vector<string> cols = OVStringHelper::Split(line, ',');
        if (!cols.size()) {
            continue;
        }
        
        string s = cols[0];        
        RomanizationSyllable tl;
        tl.setInputType(TLSyllable);
        char lastChar = 0;
        for (string::iterator i = s.begin(), e = s.end(); i != e; ++i) {
            lastChar = *i;
            tl.insertCharacterAtCursor(lastChar);
        }
        
        s = tl.convertToPOJSyllable().normalizedQueryData();
        RomanizationSyllable poj;
        lastChar = 0;
        for (string::iterator i = s.begin(), e = s.end(); i != e; ++i) {
            lastChar = *i;
            poj.insertCharacterAtCursor(lastChar);
        }
        
        int tone = 2;
        if (lastChar == 'p' || lastChar == 't' || lastChar == 'k' || lastChar == 'h') {
            tone = 8;
        }        
        poj.normalize(tone);
        
        cout << poj.normalizedQueryData(tone) << "," << poj.composedForm() << "\n";
    }
    
    
    return 0;
}
