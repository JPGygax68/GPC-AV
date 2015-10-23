#pragma once

#include <memory>
#include <string>

namespace gpc {
    
    namespace av {
        
        class Source {
        public:

            Source();
            Source(Source&& orig);
            ~Source();

            void open(const std::string &url);

            /* Open a source by its URL.
             */
            static auto create(const std::string &url) -> Source;

        private:

            struct Private;

            std::unique_ptr<Private>    p;
        };

    } // ns av
    
} // ns gpc