//
// BlockReadingBuilder.h
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

#ifndef BlockReadingBuilder_h
#define BlockReadingBuilder_h

#include <vector>
#include "Grid.h"
#include "LanguageModel.h"

namespace Formosa {
    namespace Gramambular {
        using namespace std;
        
        class BlockReadingBuilder {
        public:
            BlockReadingBuilder(LanguageModel *inLM);
            void clear();
            void setCursor(size_t inNewIndex);
            void insertReadingAtCursor(const string& inReading);
            bool deleteReadingBeforeCursor();   // backspace
            bool deleteReadingAfterCursor();    // delete
                        
        protected:
            void build();
            
            static const string Join(vector<string>::const_iterator begin, vector<string>::const_iterator end);
            
            static const size_t MaximumBuildSpanLength = 4;
            
            size_t m_cursorIndex;
            vector<string> m_readings;
            
            Grid m_grid;
            LanguageModel *m_LM;
        };
        
        inline BlockReadingBuilder::BlockReadingBuilder(LanguageModel *inLM)
            : m_LM(inLM)
        {
        }
        
        inline void BlockReadingBuilder::clear()
        {
            m_cursorIndex = 0;
            m_readings.clear();
            m_grid.clear();
        }

        inline void BlockReadingBuilder::setCursor(size_t inNewIndex)
        {
            m_cursorIndex = inNewIndex > m_readings.size() ? m_readings.size() : inNewIndex;
        }

        
        inline void BlockReadingBuilder::insertReadingAtCursor(const string& inReading)
        {
            m_readings.insert(m_readings.begin() + m_cursorIndex, inReading);
                                    
            m_grid.expandGridByOneAtLocation(m_cursorIndex);            
            build();
            m_cursorIndex++;   
        }
        
        inline bool BlockReadingBuilder::deleteReadingBeforeCursor()
        {
            if (!m_cursorIndex) {
                return false;
            }
            
            m_readings.erase(m_readings.begin() + m_cursorIndex - 1, m_readings.begin() + m_cursorIndex);
            m_cursorIndex--;
            m_grid.shrinkGridByOneAtLocation(m_cursorIndex);
            build();
            return true;
        }
        
        inline bool BlockReadingBuilder::deleteReadingAfterCursor()
        {
            if (m_cursorIndex == m_readings.size()) {
                return false;
            }
            
            m_readings.erase(m_readings.begin() + m_cursorIndex, m_readings.begin() + m_cursorIndex + 1);
            m_grid.shrinkGridByOneAtLocation(m_cursorIndex);
            build();
            return true;
        }
        
        inline void BlockReadingBuilder::build()
        {
            if (!m_LM) {
                return;
            }
            
            size_t begin = 0;
            size_t end = m_cursorIndex + MaximumBuildSpanLength;
            
            if (m_cursorIndex < MaximumBuildSpanLength) {
                begin = 0;
            }
            else {
                begin = m_cursorIndex - MaximumBuildSpanLength;
            }
            
            if (end > m_readings.size()) {
                end = m_readings.size();
            }
            
            for (size_t p = begin ; p < end ; p++) {
                for (size_t q = 1 ; q <= MaximumBuildSpanLength ; p+q <= end) {                    
                    string combinedReading = Join(m_readings.begin() + p, m_readings.begin() + q);
                    
                    if (m_LM->hasUnigramsForKey(combinedReading)) {
                        cerr << "placing reading: " << combinedReading << ", at (" << p << "," << q << ")" << endl;
                        
                        Node n(combinedReading, m_LM->unigramsForKeys(combinedReading), vector<Bigram>());                        
                        m_grid.insertNode(n, p, q);
                    }
                }
            }
        }
        
        const string BlockReadingBuilder::Join(vector<string>::const_iterator begin, vector<string>::const_iterator end)
        {
            string result;
            for (vector<string>::const_iterator iter = begin ; iter != end ; ++iter) {
                result += *iter;
            }
            return result;
        }                    
    };
};

#endif
