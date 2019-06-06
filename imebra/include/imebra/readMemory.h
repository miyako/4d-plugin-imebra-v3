/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file readMemory.h
    \brief Declaration of the class ReadMemory.

*/

#if !defined(imebraReadMemory__INCLUDED_)
#define imebraReadMemory__INCLUDED_

#include <memory>
#include <string>
#include "definitions.h"

#ifndef SWIG

namespace imebra
{
namespace implementation
{
class memory;
}
}
#endif

namespace imebra
{

///
/// \brief Manages a read-only buffer of memory.
///
/// The buffer of memory is usually associated with a Tag buffer content.
///
/// The memory used by ReadMemory and ReadWriteMemory is managed by MemoryPool.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API ReadMemory
{
    ReadMemory(const ReadMemory&) = delete;
    ReadMemory& operator=(const ReadMemory&) = delete;

#ifndef SWIG
    friend class MemoryStreamInput;
    friend class ReadWriteMemory;
    friend class ReadingDataHandlerNumeric;
private:
    explicit ReadMemory(std::shared_ptr<const implementation::memory> pMemory);
#endif


public:
    /// \brief Construct an empty buffer of memory.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ReadMemory();

    /// \brief Construct a buffer of memory and copy the specified content into it.
    ///
    /// <b>Java</b>
    ///
    /// In Java this method accepts a single parameter (a byte array).
    ///
    /// <b>Python</b>
    ///
    /// In Python this method accepts a single parameter (an array).
    ///
    /// \param source      a pointer to the source data
    /// \param sourceSize  the amount of data to copy into the allocated memory
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ReadMemory(const char* source, size_t sourceSize);

    virtual ~ReadMemory();

    /// \brief Return the memory size, in bytes.
    ///
    /// \return the memory size in bytes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t size() const;

    /// \brief Copies the raw memory content into the specified buffer.
    ///
    /// If the allocated buffer is not large enough then the method doesn't
    ///  copy any data and just returns the required buffer' size.
    ///
    /// <b>Java</b>
    ///
    /// In Java this method accepts a single parameter (a byte array).
    /// The size of the byte array must be equal or greater than the number of
    /// bytes stored by the data handler.
    ///
    /// <b>Python</b>
    ///
    /// In Python this method accepts a single parameter (an array).
    /// The size of the array (in bytes) must be equal or greater than the number
    /// of bytes stored by the data handler.
    ///
    /// \param destination     a pointer to the allocated buffer
    /// \param destinationSize the size of the allocated buffer, in bytes
    /// \return the number of bytes copied into the pre-allocated buffer, or the
    ///         desired size of destination if destinationSize is smaller than
    ///         the return value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t data(char* destination, size_t destinationSize) const;

    /// \brief Copies the raw memory region into the specified buffer.
    ///
    /// If the memory's size is too small to extract the requested region then
    /// throws MemorySizeError.
    ///
    /// <b>Java</b>
    ///
    /// In Java this method accepts two parameters:
    /// - a byte array
    /// - a long integer (sourceOffset)
    ///
    /// <b>Python</b>
    ///
    /// In Python this method accepts two parameters:
    /// - an array
    /// - an integer (sourceOffset)
    ///
    /// \param destination     a pointer to the allocated buffer
    /// \param destinationSize the size of the allocated buffer, in bytes
    /// \param sourceOffset    the offset
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void regionData(char* destination, size_t destinationSize, size_t sourceOffset) const;

#ifndef SWIG
    /// \brief Return a pointer to the constant referenced memory.
    ///
    /// The referenced buffer is owned by the ReadMemory object and must
    /// not be freed by the client.
    ///
    /// \param pDataSize pointer to a variable that will be filled with the
    ///        memory size, in bytes
    /// \return a pointer to the referenced memory.
    ///        The referenced buffer is owned by the ReadMemory object and must
    ///        not be freed by the client.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const char* data(size_t* pDataSize) const;
#endif

    /// \brief Return true if the referenced memory is zero bytes long or hasn't
    ///        been allocated yet.
    ///
    /// \return true if the referenced memory does not exist or is empty
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool empty() const;

#ifndef SWIG
protected:
    std::shared_ptr<const implementation::memory> m_pMemory;
#endif
};

}

#endif // !defined(imebraReadMemory__INCLUDED_)
