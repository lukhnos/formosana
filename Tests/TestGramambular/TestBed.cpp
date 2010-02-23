//
// TestBed.cpp
//
// Copyright (c) 2007-2010 Lukhnos D. Liu (http://lukhnos.org)
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

#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include "OVStringHelper.h"
#include "Gramambular.h"

using namespace OpenVanilla;
using namespace std;
using namespace Formosa::Gramambular;

class SimpleLM : public LanguageModel
{
public:
    SimpleLM(const string& inPath)
    {
        ifstream ifs;
        ifs.open("SampleData.txt");
        while (ifs.good()) {
            string line;
            getline(ifs, line);
            vector<string> p = OVStringHelper::Split(line, ' ');
            
            if (p.size() == 3) {
                Unigram u;
                u.keyValue.key = p[0];
                u.keyValue.value = p[1];
                u.score = atof(p[2].c_str());
                
                m_db[p[0]].push_back(u);
            }
        }
        ifs.close();        
    }

    virtual const vector<Bigram> bigramsForKeys(const string &preceedingKey, const string& key)
    {
        vector<Bigram>();
    }
    
    virtual const vector<Unigram> unigramsForKeys(const string &key)
    {
        map<string, vector<Unigram> >::const_iterator f = m_db.find(key);
        return f == m_db.end() ? vector<Unigram>() : (*f).second;
    }
    
    virtual bool hasUnigramsForKey(const string& key)
    {
        map<string, vector<Unigram> >::const_iterator f = m_db.find(key);
        return f != m_db.end();
    }
    
protected:
    map<string, vector<Unigram> > m_db;    
};

int main()
{
    SimpleLM lm("SampleData.txt");
    
    BlockReadingBuilder builder(&lm);
    
    builder.insertReadingAtCursor("ㄍㄠ");
    builder.insertReadingAtCursor("ㄐㄧˋ");
    
    builder.setCursor(1);
    builder.insertReadingAtCursor("ㄎㄜ");
    
    builder.setCursor(0);
    builder.deleteReadingAfterCursor();
    
    cout << builder.grid().dumpDOT() << endl;
    
    return 0;
}
