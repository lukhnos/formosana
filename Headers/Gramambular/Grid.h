//
// Grid.h
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

#ifndef Grid_h
#define Grid_h

#include <map>
#include "Span.h"

namespace Formosa {
    namespace Gramambular {
        class Grid {
        public:
            void clear();
            void insertNode(const Node& inNode, size_t inLocation, size_t inSpanningLength);

        protected:
            vector<Span> m_spans;
        };
        
        inline void Grid::clear()
        {
            m_spans.clear();
        }
        
        inline void Grid::insertNode(const Node& inNode, size_t inLocation, size_t inSpanningLength)
        {            
            if (inLocation >= m_spans.size()) {
                size_t diff = inLocation - m_spans.size() + 1;
                
                for (size_t i = 0 ; i < diff ; i++) {
                    m_spans.push_back(Span());
                }
            }

            m_spans[inLocation].insertNodeOfLength(inNode, inSpanningLength);
        }
    };
};

#endif
