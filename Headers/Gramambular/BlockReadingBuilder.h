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
            size_t m_cursorIndex;
            vector<string> m_readings;
            
            Grid m_grid;
            const LanguageModel *m_LM;
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
            m_cursorIndex++;   
        }
        
        inline bool BlockReadingBuilder::deleteReadingBeforeCursor()
        {
            if (!m_cursorIndex) {
                return false;
            }
            
            m_readings.erase(m_readings.begin() + m_cursorIndex - 1, m_readings.begin() + m_cursorIndex);
            m_cursorIndex--;
            return true;
        }
        
        inline bool BlockReadingBuilder::deleteReadingAfterCursor()
        {
            if (m_cursorIndex == m_readings.size()) {
                return false;
            }
            
            m_readings.erase(m_readings.begin() + m_cursorIndex, m_readings.begin() + m_cursorIndex + 1);
            return true;
        }
        
    };
};

#endif
