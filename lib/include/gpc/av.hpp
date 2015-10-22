#pragma once

#include <string>

namespace gpc {
    
    namespace av {
        
        class Source {
        public:

            /* Open a stream by its URL.
             */
            static auto open(const std::string &url) -> Source;

        };

    } // ns av
    
} // ns gpc