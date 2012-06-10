#include "TaiwaneseRomanization.h"
#include "OVStringHelper.h"
#include <iostream>
#include <cctype>

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
        RomanizationSyllable h;
        h.setInputType(HakkaPFSSyllable);
        for (string::iterator i = s.begin(), e = s.end(); i != e; ++i) {
            if (isnumber(*i)) {
                h.normalize((*i) - '0');
                break;
            }
            else {
                h.insertCharacterAtCursor(*i);
            }
        }
        
        cout << s << "," << h.composedForm() << "\n";
    }
    
    
    return 0;
}
