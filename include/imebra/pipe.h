/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file pipe.h
    \brief Declaration of the Pipe class.

*/

#if !defined(pipe__INCLUDED_)
#define pipe__INCLUDED_

#include <string>
#include "baseStreamInput.h"
#include "baseStreamOutput.h"
#include "definitions.h"

namespace imebra
{

class ReadMemory;
class ReadWriteMemory;

namespace implementation
{
class pipeSequenceStream;
}

///
/// \brief A Pipe can be used to push and pull data to/from an Imebra codec.
///
/// This is useful when an Imebra codec must be used with a data source
/// or a data sink not supported by the library (e.g. a TLS stream).
///
/// In order to allow Imebra to read data from a custom data source:
/// - allocate a Pipe class and use it as parameter for the StreamReader
///   needed by the codec
/// - from a secondary thread feed the data to the data source by calling
///   feed()
///
/// In order to allow Imebra to write data to a custom data source:
/// - allocate a Pipe class and use it as parameter for the StreamWriter
///   needed by the codec
/// - from a secondary thread read the data to feed to the data sink by
///   calling sink()
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API Pipe: public BaseStreamInput, public BaseStreamOutput
{
    Pipe(const Pipe&) = delete;
    Pipe& operator=(const Pipe&) = delete;

public:
    /// \brief Constructor
    ///
    /// \param circularBufferSize the size of the buffer that stores the data
    ///                           fed to the Pipe until it is fetched
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit Pipe(size_t circularBufferSize);

    ///
    /// \brief Copy data into the Pipe so it can be retrieved by a
    ///        codec via a StreamReader.
    ///
    /// \param buffer the data to copy into the Pipe's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void feed(const ReadMemory& buffer);

    ///
    /// \brief Read data from the Pipe. The data can be pushed into the pipe via
    ///        a StreamWriter.
    ///
    /// \param buffer   a buffer that will contain the read data. The method will
    ///                 try to read as many bytes as the buffer size
    /// \return the number of bytes read from the pipe, which may be less than the
    ///         buffer's size
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t sink(ReadWriteMemory& buffer);

    ///
    /// \brief Wait for the specified amount of time or until the internal buffer
    ///        is empty (all the data fed has been retrieved by the StreamReader
    ///        or by sink()), whichever comes first, then call terminate().
    ///
    /// Subsequent read and write operations will fail by throwing the
    /// exception StreamClosedError.
    ///
    /// \param timeoutMilliseconds the maximum time to wait until the internal
    ///                            buffer is empty, in milliseconds
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void close(unsigned int timeoutMilliseconds);

    ///
    /// \brief Instruct any pending operation to terminate.
    ///
    /// Current and subsequent read and write operations will fail by
    /// throwing the exception StreamClosedError.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void terminate();
};


}
#endif // !defined(pipe__INCLUDED_)
