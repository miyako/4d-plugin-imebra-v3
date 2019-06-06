/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file readWriteMemory.h
    \brief Declaration of the class ReadWriteMemory.

*/

#if !defined(imebraReadWriteMemory__INCLUDED_)
#define imebraReadWriteMemory__INCLUDED_

#include <memory>
#include <string>
#include "definitions.h"
#include "readMemory.h"

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
/// \brief Manages a writable buffer of memory.
///
/// The buffer of memory is usually associated with a Tag buffer content.
///
/// The memory used by ReadMemory and ReadWriteMemory is managed by MemoryPool.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API ReadWriteMemory: public ReadMemory
{
    ReadWriteMemory(const ReadWriteMemory&) = delete;
    ReadWriteMemory& operator=(const ReadWriteMemory&) = delete;

#ifndef SWIG
    friend class MemoryStreamOutput;
    friend class DrawBitmap;
    friend class WritingDataHandlerNumeric;
private:
    explicit ReadWriteMemory(std::shared_ptr<implementation::memory> pMemory);
#endif

public:

    /// \brief Construct an empty buffer of memory.
    ///
    /// The memory can be resized later with resize().
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ReadWriteMemory();

    /// \brief Construct a buffer of memory of the specified size.
    ///
    /// The memory can be resized later with resize().
    ///
    /// \param initialSize the initial memory's size, in bytes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ReadWriteMemory(size_t initialSize);

    /// \brief Construct a buffer of memory and copy the specified content into it.
    ///
    /// \param sourceMemory the object containing the initial memory's content
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ReadWriteMemory(const ReadMemory& sourceMemory);

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
    explicit ReadWriteMemory(const char* source, size_t sourceSize);

    virtual ~ReadWriteMemory();

    /// \brief Copy the content from another memory object.
    ///
    /// \param sourceMemory the source memory object
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void copyFrom(const ReadMemory& sourceMemory);

    /// \brief Resize the memory to zero bytes.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clear();

    /// \brief Resize the memory.
    ///
    /// \param newSize the new memory size, in bytes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void resize(size_t newSize);

    /// \brief Reserve the specified amount of bytes, without changing the memory
    ///        size
    ///
    /// \param reserveSize the number of bytes to reserve
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void reserve(size_t reserveSize);

#ifndef SWIG
    /// \brief Return a pointer to the referenced memory.
    ///
    /// The referenced buffer is owned by the ReadWriteMemory object and must
    /// not be freed by the client.
    ///
    /// \param pDataSize pointer to a variable that will be filled with the
    ///        memory size, in bytes
    /// \return a pointer to the referenced memory.
    ///        The referenced buffer is owned by the ReadWriteMemory object and
    ///        must not be freed by the client.
    ///
    ///
    ///////////////////////////////////////////////////////////////////////////////
    char* data(size_t* pDataSize) const;
#endif

    /// \brief Copy the content of the specified buffer into the ReadWriteMemory
    ///        object.
    ///
    /// <b>Java</b>
    ///
    /// In Java this method accepts a single parameter (a byte array).
    ///
    /// <b>Python</b>
    ///
    /// In Python this method accepts a single parameter (an array).
    ///
    /// \param source     a pointer to the source memory buffer
    /// \param sourceSize the number of bytes to copy and the new memory size
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void assign(const char* source, size_t sourceSize);

    /// \brief Copy the content of the specified buffer into a region of the
    ///        ReadWriteMemory.
    ///
    /// The memory size remains unchanged.
    ///
    /// <b>Java</b>
    ///
    /// In Java this method accepts two parameters:
    /// - a byte array
    /// - a long integer (destinationOffset)
    ///
    /// <b>Python</b>
    ///
    /// In Python this method accepts two parameters:
    /// - an array
    /// - an integer (destinationOffset)
    ///
    /// \param source     a pointer to the source memory buffer
    /// \param sourceSize the number of bytes to copy
    /// \param destinationOffset the offset into the ReadWriteMemory at which the
    ///                   data must be copied
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void assignRegion(const char* source, size_t sourceSize, size_t destinationOffset);
};

}

#endif // !defined(imebraReadWriteMemory__INCLUDED_)
