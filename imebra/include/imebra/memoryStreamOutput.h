/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/


#if !defined(imebraMemoryStreamOutput__INCLUDED_)
#define imebraMemoryStreamOutput__INCLUDED_

#include <string>
#include "baseStreamOutput.h"
#include "definitions.h"

namespace imebra
{

class ReadWriteMemory;

///
/// \brief An output stream that writes data into a memory region.
///
/// When new data is written into the memory then it is resized as necessary.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API MemoryStreamOutput : public BaseStreamOutput
{
    MemoryStreamOutput(const MemoryStreamOutput&) = delete;
    MemoryStreamOutput& operator=(const MemoryStreamOutput&) = delete;

public:
    /// \brief Constructor.
    ///
    /// \param memory the memory region into which the stream will write the data
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit MemoryStreamOutput(const ReadWriteMemory& memory);

    virtual ~MemoryStreamOutput();
};

}
#endif // !defined(imebraMemoryStreamOutput__INCLUDED_)
