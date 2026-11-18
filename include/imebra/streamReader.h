/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file streamReader.h
    \brief Declaration of the the class used to read the streams.

*/

#if !defined(imebraStreamReader__INCLUDED_)
#define imebraStreamReader__INCLUDED_

#ifndef SWIG

#include <memory>
#include "definitions.h"

namespace imebra
{
namespace implementation
{
class streamReader;
}

}

#endif


namespace imebra
{

class BaseStreamInput;

class CodecFactory;

///
/// \brief A StreamReader is used to read data from a BaseStreamInput
///        object.
///
/// A StreamReader can be mapped to only a portion of the BaseStreamInput it
/// manages: for instance this is used by the Imebra classes to read Jpeg
/// streams embedded into a DICOM stream.
///
/// \warning  The StreamReader object IS NOT THREAD-SAFE: however, several
///           StreamReader objects from different threads can be connected to
///           the same BaseStreamInput object.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API StreamReader
{
    StreamReader(const StreamReader&) = delete;
    StreamReader& operator=(const StreamReader&) = delete;

#ifndef SWIG
    friend class CodecFactory;
    friend class Tag;
    friend class AssociationSCU;
    friend class AssociationSCP;

private:
    explicit StreamReader(std::shared_ptr<implementation::streamReader> pReader);
#endif

public:
    /// \brief Constructor.
    ///
    /// \param stream the BaseStreamInput object from which the StreamReader will
    ///               read
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit StreamReader(const BaseStreamInput& stream);

    /// \brief Constructor.
    ///
    /// This version of the constructor limits the portion of the stream that
    /// the StreamReader will see.
    ///
    /// \param stream        the BaseStreamInput object from which the StreamReader
    ///                      will read
    /// \param virtualStart  the first visible byte of the managed stream
    /// \param virtualLength the number of visible bytes in the managed stream
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit StreamReader(const BaseStreamInput& stream, size_t virtualStart, size_t virtualLength);

    ///
    /// \brief Returns a virtual stream that has a restricted view into the
    ///        stream.
    ///
    /// The reading position of this stream advances to the end of the virtual
    /// stream.
    ///
    /// \param virtualStreamLength the number of bytes that the virtual
    ///                            stream can read
    /// \return a virtual stream that sees a limited number of bytes of this
    ///         stream
    ///
    ///////////////////////////////////////////////////////////////////////////////
    StreamReader* getVirtualStream(size_t virtualStreamLength);

    ///
    /// \brief Cause the controlled stream to throw StreamClosedError during the
    ///        mext read operation.
    ///
    /// This can be used to cause reading threads to terminate.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void terminate();

    virtual ~StreamReader();

#ifndef SWIG
protected:
    std::shared_ptr<implementation::streamReader> m_pReader;
#endif
};

}

#endif // !defined(imebraStreamReader__INCLUDED_)
