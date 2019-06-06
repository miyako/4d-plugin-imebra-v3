/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/// \file readingDataHandler.h
/// \brief Declaration of the class ReadingDataHandler.
///
///////////////////////////////////////////////////////////////////////////////

#if !defined(imebraReadingDataHandler__INCLUDED_)
#define imebraReadingDataHandler__INCLUDED_

#include <string>
#include <memory>
#include "definitions.h"
#include "readMemory.h"

#ifndef SWIG

namespace imebra
{
namespace implementation
{
namespace handlers
{
class readingDataHandler;
}
}
}

#endif


namespace imebra
{

///
/// \brief The ReadingDataHandler class allows reading the content
///        of a Dicom Tag.
///
/// ReadingDataHandler is able to return the Tag's content as a string,
/// a number, a date/time or an age.
///
/// In order to obtain a ReadingDataHandler object for a specific Tag stored
/// in a DataSet, call DataSet::getReadingDataHandler() or
/// Tag::getReadingDataHandler().
///
/// The ReadingDataHandler object keeps a reference to the buffer's memory:
/// the buffer's memory is never modified but only replaced by a new memory
/// area, therefore the ReadingDataHandler client does not need to worry about
/// other clients modifying the memory being read.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API ReadingDataHandler
{
    ReadingDataHandler(const ReadingDataHandler&) = delete;
    ReadingDataHandler& operator=(const ReadingDataHandler&) = delete;

#ifndef SWIG
    friend class DataSet;
    friend class Tag;
    friend class ReadingDataHandlerNumeric;

private:
    explicit ReadingDataHandler(std::shared_ptr<imebra::implementation::handlers::readingDataHandler> pDataHandler);
#endif

public:

    virtual ~ReadingDataHandler();

    /// \brief Returns the number of elements in the Tag's buffer handled by the
    ///        data handler.
    ///
    /// If the ReadingDataHandler object is related to a buffer that contains
    /// strings then it returns the number of strings stored in the buffer.
    /// Multiple strings are separated by a separator char.
    ///
    /// \return the number of elements stored in the handled Dicom buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t getSize() const;

    /// \brief Returns the data type (VR) of the data handled by the data handler.
    ///
    /// \return the data type of the handled data
    ///
    ///////////////////////////////////////////////////////////////////////////////
    tagVR_t getDataType() const;

    /// \brief Retrieve a buffer's value as signed long integer (32 bit).
    ///
    /// If the buffer's value cannot be converted to a signed long integer
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \return the tag's value as a signed 32 bit integer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::int32_t getSignedLong(size_t index) const;

    /// \brief Retrieve a buffer's value as an unsigned long integer (32 bit).
    ///
    /// If the buffer's value cannot be converted to an unsigned long integer
    /// then throws DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \return the tag's value as an unsigned 32 bit integer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::uint32_t getUnsignedLong(size_t index) const;

    /// \brief Retrieve a buffer's value as a double floating point value (64 bit).
    ///
    /// If the buffer's value cannot be converted to a double value then throws
    /// DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \return the tag's value as a double floating point value (64 bit)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double getDouble(size_t index) const;

    /// \brief Retrieve a buffer's value as an ASCII string.
    ///
    /// If the buffer's value cannot be converted to a string then throws
    /// DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \return the tag's value as an ASCII string
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getString(size_t index) const;

    /// \brief Retrieve a buffer's value as a Unicode string.
    ///
    /// If the buffer's value cannot be converted to a string then throws
    /// DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \return the tag's value as a Unicode string
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::wstring getUnicodeString(size_t index) const;

    /// \brief Retrieve a buffer's value a date or time.
    ///
    /// If the buffer's value cannot be converted to a date or time then throws
    /// DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \return the tag's value as a date or time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Date getDate(size_t index) const;

    /// \brief Retrieve a buffer's value as an Age.
    ///
    /// If the buffer's value cannot be converted to an Age then throws
    /// DataHandlerConversionError.
    ///
    /// \param index the element number within the buffer. Must be smaller than
    ///        getSize()
    /// \return the tag's value as an Age
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Age getAge(size_t index) const;

#ifndef SWIG
protected:
    std::shared_ptr<imebra::implementation::handlers::readingDataHandler> m_pDataHandler;
#endif
};

}

#endif // !defined(imebraReadingDataHandler__INCLUDED_)
