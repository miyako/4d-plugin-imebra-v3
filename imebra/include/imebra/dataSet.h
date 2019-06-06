/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file dataSet.h
    \brief Declaration of the DataSet class.

*/

#if !defined(imebraDataSet__INCLUDED_)
#define imebraDataSet__INCLUDED_

#include "definitions.h"

#include <string>
#include <cstdint>
#include <memory>
#include "image.h"
#include "readingDataHandlerNumeric.h"
#include "writingDataHandlerNumeric.h"
#include "tagId.h"

#ifndef SWIG

namespace imebra
{
namespace implementation
{
class dataSet;
}
}

#endif

namespace imebra
{

class Tag;
class LUT;

///
///  \brief This class represents a DICOM dataset.
///
/// The information it contains is organized into groups and each group may
/// contain several tags.
///
/// You can create a DataSet from a DICOM file by using the
/// CodecFactory::load() function:
///
/// In C++:
/// \code
/// using namespace imebra;
/// std::unique_ptr<DataSet> pDataSet(CodecFactory::load("/path/to/file));
/// \endcode
///
/// In Java:
/// \code
/// com.imebra.DataSet dataSet = com.imebra.CodecFactory.load("/path/to/file");
/// \endcode
///
/// You can also create an empty DataSet that can be filled with data and
/// images and then saved to a DICOM file via CodecFactory::save().
///
/// When creating an empty DataSet you should specify the proper transfer
/// syntax in the DataSet constructor.
///
/// To retrieve the DataSet's content, use one of the following methods which
/// give direct access to the tags' values:
/// - getImage()
/// - getImageApplyModalityTransform()
/// - getSequenceItem()
/// - getSignedLong()
/// - getUnsignedLong()
/// - getDouble()
/// - getString()
/// - getUnicodeString()
/// - getAge()
/// - getDate()
///
/// In alternative, you can first retrieve a ReadingDataHandler with
/// getReadingDataHandler() and then access the tag's content via the handler.
///
/// To set the DataSet's content, use one of the following methods:
/// - setImage()
/// - setSequenceItem()
/// - setSignedLong()
/// - setUnsignedLong()
/// - setDouble()
/// - setString()
/// - setUnicodeString()
/// - setAge()
/// - setDate()
///
/// The previous methods allow to write just the first item in the tag's
/// content and before writing wipe out the old tag's content (all the items).
/// If you have to write more than one item in a tag, retrieve a
/// WritingDataHandler with getWritingDataHandler() and then modify all the
/// tag's items using the WritingDataHandler.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API DataSet
{
    DataSet(const DataSet&) = delete;
    DataSet& operator=(const DataSet&) = delete;

#ifndef SWIG
    friend class DicomDirEntry;
	friend class DicomDir;
    friend class ModalityVOILUT;
    friend class VOILUT;
    friend class CodecFactory;
    friend class Tag;
    friend class AssociationMessage;
    friend class DimseService;
    friend class DimseResponse;
    friend class CStoreCommand;
    friend class CGetCommand;
    friend class CGetResponse;
    friend class CFindCommand;
    friend class CFindResponse;
    friend class CMoveCommand;
    friend class CMoveResponse;
    friend class CEchoCommand;
    friend class CEchoResponse;
    friend class NEventReportCommand;
    friend class NEventReportResponse;
    friend class NGetCommand;
    friend class NGetResponse;
    friend class NSetCommand;
    friend class NSetResponse;
    friend class NActionCommand;
    friend class NActionResponse;
    friend class NCreateCommand;
    friend class NCreateResponse;
    friend class NDeleteCommand;
    friend class NDeleteResponse;
    friend class DimseCommandBase;

private:
    explicit DataSet(std::shared_ptr<imebra::implementation::dataSet> pDataSet);
#endif
public:
    /// \brief Construct an empty DICOM dataset with unspecified transfer syntax
    ///        (which represents the default value "1.2.840.10008.1.2" or
    ///        "Implicit VR little endian") and charset "ISO 2022 IR 6".
    ///
    /// Use this method when creating a DataSet that will be embedded as a sequence
    /// item.
    ///
    ///////////////////////////////////////////////////////////////////////////////
	DataSet();

    /// \brief Construct an empty DICOM dataset with charset "ISO 2022 IR 6" and
    ///        the desidered transfer syntax.
    ///
    /// \param transferSyntax the dataSet's transfer syntax. The following transfer
    ///                       syntaxes are supported:
    ///                       - "1.2.840.10008.1.2" (Implicit VR little endian)
    ///                       - "1.2.840.10008.1.2.1" (Explicit VR little endian)
    ///                       - "1.2.840.10008.1.2.2" (Explicit VR big endian)
    ///                       - "1.2.840.10008.1.2.5" (RLE compression)
    ///                       - "1.2.840.10008.1.2.4.50" (Jpeg baseline 8 bit
    ///                         lossy)
    ///                       - "1.2.840.10008.1.2.4.51" (Jpeg extended 12 bit
    ///                         lossy)
    ///                       - "1.2.840.10008.1.2.4.57" (Jpeg lossless NH)
    ///                       - "1.2.840.10008.1.2.4.70" (Jpeg lossless NH first
    ///                         order prediction)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit DataSet(const std::string& transferSyntax);

    /// \brief Construct an empty DICOM dataset and specifies the default charsets.
    ///
    /// \param transferSyntax the dataSet's transfer syntax. The following transfer
    ///                       syntaxes are supported:
    ///                       - "1.2.840.10008.1.2" (Implicit VR little endian)
    ///                       - "1.2.840.10008.1.2.1" (Explicit VR little endian)
    ///                       - "1.2.840.10008.1.2.2" (Explicit VR big endian)
    ///                       - "1.2.840.10008.1.2.5" (RLE compression)
    ///                       - "1.2.840.10008.1.2.4.50" (Jpeg baseline 8 bit
    ///                         lossy)
    ///                       - "1.2.840.10008.1.2.4.51" (Jpeg extended 12 bit
    ///                         lossy)
    ///                       - "1.2.840.10008.1.2.4.57" (Jpeg lossless NH)
    ///                       - "1.2.840.10008.1.2.4.70" (Jpeg lossless NH first
    ///                         order prediction)
    ///
    /// \param charsets a list of charsets supported by the DataSet
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit DataSet(const std::string& transferSyntax, const charsetsList_t& charsets);


    /// \brief Destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~DataSet();

    /// \brief Returns a list of all the tags stored in the DataSet, ordered by
    ///        group and tag ID.
    ///
    /// \return an ordered list of the stored Tags
    ///
    ///////////////////////////////////////////////////////////////////////////////
    tagsIds_t getTags() const;

    /// \brief Retrieve the Tag with the specified ID.
    ///
    /// \param tagId the ID of the tag to retrieve
    /// \return the Tag with the specified ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Tag* getTag(const TagId& tagId) const;

    /// \brief Retrieve the Tag with the specified ID or create it if it doesn't
    ///        exist.
    ///
    /// \param tagId the ID of the tag to retrieve
    /// \param tagVR the VR to use for the new tag if one doesn't exist already
    /// \return the Tag with the specified ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Tag* getTagCreate(const TagId& tagId, tagVR_t tagVR);

    /// \brief Retrieve the Tag with the specified ID or create it if it doesn't
    ///        exist.
    ///
    /// \param tagId the ID of the tag to retrieve
    /// \return the Tag with the specified ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Tag* getTagCreate(const TagId& tagId);

    /// \brief Retrieve an image from the dataset.
    ///
    /// Images should be retrieved in order (first frame 0, then frame 1, then
    /// frame 2 and so on).
    /// Images can be retrieved also in random order but this introduces
    /// performance penalties.
    ///
    /// Throws DataSetImageDoesntExistError if the requested frame does not exist.
    ///
    /// \note Images retrieved from the DataSet should be processed by the
    ///       ModalityVOILUT transform, which converts the modality-specific pixel
    ///       values to values that the application can understand. Consider using
    ///       getImageApplyModalityTransform() to retrieve the image already
    ///       processed by ModalityVOILUT.
    ///
    /// \param frameNumber the frame to retrieve (the first frame is 0)
    /// \return an Image object containing the decompressed image
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Image* getImage(size_t frameNumber);

    /// \brief Retrieve an image from the dataset and if necessary process it with
    ///        ModalityVOILUT before returning it.
    ///
    /// Images should be retrieved in order (first frame 0, then frame 1, then
    ///  frame 2 and so on).
    /// Images can be retrieved also in random order but this introduces
    ///  performance penalties.
    ///
    /// Throws DataSetImageDoesntExistError if the requested frame does not exist.
    ///
    /// \param frameNumber the frame to retrieve (the first frame is 0)
    /// \return an image object containing the decompressed image processed with
    ///         ModalityVOILUT
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Image* getImageApplyModalityTransform(size_t frameNumber);

    /// \brief Insert an image into the dataset.
    ///
    /// In multi-frame datasets the images must be inserted in order: first, insert
    ///  the frame 0, then the frame 1, then the frame 2 and so on.
    ///
    /// All the inserted images must have the same transfer syntax and the same
    ///  properties (size, color space, high bit, bits allocated).
    ///
    /// If the images are inserted in the wrong order then the
    ///  DataSetWrongFrameError exception is thrown.
    ///
    /// If the image being inserted has different properties than the ones of the
    ///  images already in the dataset then the exception
    ///  DataSetDifferentFormatError is thrown.
    ///
    /// \param frameNumber    the frame number (the first frame is 0)
    /// \param image          the image
    /// \param quality        the quality to use for lossy compression. Ignored
    ///                        if lossless compression is used
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setImage(size_t frameNumber, const Image& image, imageQuality_t quality);

    /// \brief Return the list of VOI settings stored in the DataSet.
    ///
    /// Each VOI setting includes the center & width values that can be used with
    /// the VOILUT transform to highlight different parts of an Image.
    ///
    /// \return a list of VOIDescription objects defined in the DataSet
    ///
    ///////////////////////////////////////////////////////////////////////////////
    vois_t getVOIs();

    /// \brief Retrieve a sequence item stored in a tag.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// If the specified Tag does not contain the specified sequence item then
    ///  throws MissingItemError.
    ///
    /// \param tagId  the tag's id containing the sequence item
    /// \param itemId the sequence item to retrieve. The first item has an Id = 0
    /// \return the requested sequence item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    DataSet* getSequenceItem(const TagId& tagId, size_t itemId);

    /// \brief Set a sequence item.
    ///
    /// If the specified Tag does not exist then creates a new one with VR
    ///  tagVR_t::SQ.
    ///
    /// \param tagId  the tag's id in which the sequence must be stored
    /// \param itemId the sequence item to set. The first item has an Id = 0
    /// \param item   the DataSet to store as a sequence item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSequenceItem(const TagId& tagId, size_t itemId, const DataSet& item);

    /// \brief Retrieve a LUT stored in a sequence item.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// If the specified Tag does not contain the specified sequence item then
    ///  throws MissingItemError.
    ///
    /// \param tagId  the tag's id containing the sequence that stores the LUTs
    /// \param itemId the sequence item to retrieve. The first item has an Id = 0
    /// \return the LUT stored in the requested sequence item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    LUT* getLUT(const TagId& tagId, size_t itemId);

    /// \brief Retrieve a ReadingDataHandler object connected to a specific
    ///        tag's buffer.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// If the specified Tag does not contain the specified buffer item then
    ///  throws MissingBufferError.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the buffer to connect to the ReadingDataHandler object.
    ///                 The first buffer has an Id = 0
    /// \return a ReadingDataHandler object connected to the requested Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ReadingDataHandler* getReadingDataHandler(const TagId& tagId, size_t bufferId) const;

    /// \brief Retrieve a WritingDataHandler object connected to a specific
    ///        tag's buffer.
    ///
    /// If the specified Tag does not exist then it creates a new tag with the VR
    ///  specified in the tagVR parameter
    ///
    /// The returned WritingDataHandler is connected to a new buffer which is
    /// updated and stored into the tag when WritingDataHandler is destroyed.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the position where the new buffer has to be stored in the
    ///                 tag. The first buffer position is 0
    /// \param tagVR    the tag's VR
    /// \return a WritingDataHandler object connected to a new Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler* getWritingDataHandler(const TagId& tagId, size_t bufferId, tagVR_t tagVR);

    /// \brief Retrieve a WritingDataHandler object connected to a specific
    ///        tag's buffer.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// The returned WritingDataHandler is connected to a new buffer which is
    /// updated and stored into the tag when WritingDataHandler is destroyed.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the position where the new buffer has to be stored in the
    ///                 tag. The first buffer position is 0
    /// \return a WritingDataHandler object connected to a new Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandler* getWritingDataHandler(const TagId& tagId, size_t bufferId);

    /// \brief Retrieve a getReadingDataHandlerNumeric object connected to a
    ///        specific tag's numeric buffer.
    ///
    /// If the tag's VR is not a numeric type then throws std::bad_cast.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// If the specified Tag does not contain the specified buffer item then
    ///  throws MissingItemError.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the buffer to connect to the ReadingDataHandler object.
    ///                 The first buffer has an Id = 0
    /// \return a ReadingDataHandlerNumeric object connected to the requested
    ///         Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ReadingDataHandlerNumeric* getReadingDataHandlerNumeric(const TagId& tagId, size_t bufferId) const;

    /// \brief Retrieve a getReadingDataHandlerNumeric object connected to a
    ///        specific tag's buffer, no matter what the tag's data type.
    ///
    /// If the tag's VR is not a numeric type then throws std::bad_cast.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// If the specified Tag does not contain the specified buffer item then
    ///  throws MissingItemError.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the buffer to connect to the ReadingDataHandler object.
    ///                 The first buffer has an Id = 0
    /// \return a ReadingDataHandlerNumeric object connected to the requested
    ///         Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ReadingDataHandlerNumeric* getReadingDataHandlerRaw(const TagId& tagId, size_t bufferId) const;

    /// \brief Retrieve a WritingDataHandlerNumeric object connected to a specific
    ///        tag's buffer.
    ///
    /// If the tag's VR is not a numeric type then throws std::bad_cast.
    ///
    /// If the specified Tag does not exist then it creates a new tag with the VR
    ///  specified in the tagVR parameter
    ///
    /// The returned WritingDataHandlerNumeric is connected to a new buffer which
    /// is updated and stored into the tag when WritingDataHandlerNumeric is
    /// destroyed.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the position where the new buffer has to be stored in the
    ///                 tag. The first buffer position is 0
    /// \param tagVR    the tag's VR
    /// \return a WritingDataHandlerNumeric object connected to a new Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandlerNumeric* getWritingDataHandlerNumeric(const TagId& tagId, size_t bufferId, tagVR_t tagVR);

    /// \brief Retrieve a WritingDataHandlerNumeric object connected to a specific
    ///        tag's buffer.
    ///
    /// If the tag's VR is not a numeric type then throws std::bad_cast.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// The returned WritingDataHandlerNumeric is connected to a new buffer which
    /// is updated and stored into the tag when WritingDataHandlerNumeric is
    /// destroyed.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the position where the new buffer has to be stored in the
    ///                 tag. The first buffer position is 0
    /// \return a WritingDataHandlerNumeric object connected to a new Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandlerNumeric* getWritingDataHandlerNumeric(const TagId& tagId, size_t bufferId);

    /// \brief Retrieve a WritingDataHandlerNumeric object connected to a specific
    ///        tag's buffer. The handler content is cast to bytes
    ///
    /// If the tag's VR is not a numeric type then throws std::bad_cast.
    ///
    /// If the specified Tag does not exist then it creates a new tag with the VR
    ///  specified in the tagVR parameter
    ///
    /// The returned WritingDataHandlerNumeric is connected to a new buffer which
    /// is updated and stored into the tag when WritingDataHandlerNumeric is
    /// destroyed.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the position where the new buffer has to be stored in the
    ///                 tag. The first buffer position is 0
    /// \param tagVR    the tag's VR
    /// \return a WritingDataHandlerNumeric object connected to a new Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandlerNumeric* getWritingDataHandlerRaw(const TagId& tagId, size_t bufferId, tagVR_t tagVR);

    /// \brief Retrieve a WritingDataHandlerNumeric object connected to a specific
    ///        tag's buffer. The handler content is cast to bytes
    ///
    /// If the tag's VR is not a numeric type then throws std::bad_cast.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// The returned WritingDataHandlerNumeric is connected to a new buffer which
    /// is updated and stored into the tag when WritingDataHandlerNumeric is
    /// destroyed.
    ///
    /// \param tagId    the tag's id containing the requested buffer
    /// \param bufferId the position where the new buffer has to be stored in the
    ///                 tag. The first buffer position is 0
    /// \return a WritingDataHandlerNumeric object connected to a new Tag's buffer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandlerNumeric* getWritingDataHandlerRaw(const TagId& tagId, size_t bufferId);

    /// \brief Check if the specified tag and tag's buffer exist.
    ///
    /// \return true if the specified tag and tag's buffer exist, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool bufferExists(const TagId& tagId, size_t bufferId);

    /// \brief Retrieve a tag's value as signed long integer (32 bit).
    ///
    /// If the tag's value cannot be converted to a signed long integer
    /// then throws DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// \param tagId    the tag's id
    /// \param elementNumber the element number within the buffer
    /// \return the tag's value as a signed 32 bit integer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::int32_t getSignedLong(const TagId& tagId, size_t elementNumber) const;

    /// \brief Retrieve a tag's value as signed long integer (32 bit).
    ///
    /// If the tag's value cannot be converted to a signed long integer
    /// then throws DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist or it does not contain the specified
    /// buffer then returns the default value specified in the parameter.
    ///
    /// \param tagId         the tag's id
    /// \param elementNumber the element number within the buffer
    /// \param defaultValue  the value to return if the tag doesn't exist
    /// \return the tag's value as a signed 32 bit integer, or defaultValue if
    ///         the tag doesn't exist
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::int32_t getSignedLong(const TagId& tagId, size_t elementNumber, std::int32_t defaultValue) const;

    /// \brief Write a new signed 32 bit integer value into the element 0 of the
    ///        specified Tag's buffer 0.
    ///
    /// If the specified Tag or buffer don't exist then a new tag is created
    /// using the specified data type (VR).
    ///
    /// \param tagId    the tag's id
    /// \param newValue the value to write into the tag
    /// \param tagVR    the tag's type to use when a new tag is created.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSignedLong(const TagId& tagId, std::int32_t newValue, tagVR_t tagVR);

    /// \brief Write a new signed 32 bit integer value into the element 0 of the
    ///        specified Tag's buffer 0.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// \param tagId    the tag's id
    /// \param newValue the value to write into the tag
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSignedLong(const TagId& tagId, std::int32_t newValue);

    /// \brief Retrieve a tag's value as unsigned long integer (32 bit).
    ///
    /// If the tag's value cannot be converted to an unsigned long integer
    /// then throws DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// \param tagId    the tag's id
    /// \param elementNumber the element number within the buffer
    /// \return the tag's value as an unsigned 32 bit integer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::uint32_t getUnsignedLong(const TagId& tagId, size_t elementNumber) const;

    /// \brief Retrieve a tag's value as unsigned long integer (32 bit).
    ///
    /// If the tag's value cannot be converted to an unsigned long integer
    /// then throws DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then returns the default value
    /// specified in the parameter.
    ///
    /// \param tagId         the tag's id
    /// \param elementNumber the element number within the buffer
    /// \param defaultValue  the value to return if the tag doesn't exist
    /// \return the tag's value as an unsigned 32 bit integer, or defaultValue if
    ///         the tag doesn't exist
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::uint32_t getUnsignedLong(const TagId& tagId, size_t elementNumber, std::uint32_t defaultValue) const;

    /// \brief Write a new unsigned 32 bit integer value into the element 0 of the
    ///        specified Tag's buffer 0.
    ///
    /// If the specified Tag doesn't exist then a new tag is created using
    /// the specified data type (VR).
    ///
    /// \param tagId    the tag's id
    /// \param newValue the value to write into the tag
    /// \param tagVR    the tag's type to use when a new tag is created.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setUnsignedLong(const TagId& tagId, std::uint32_t newValue, tagVR_t tagVR);

    /// \brief Write a new unsigned 32 bit integer value into the element 0 of the
    ///        specified Tag's buffer 0.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// \param tagId    the tag's id
    /// \param newValue the value to write into the tag
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setUnsignedLong(const TagId& tagId, std::uint32_t newValue);

    /// \brief Retrieve a tag's value as a 64 bit floating point.
    ///
    /// If the tag's value cannot be converted to a floating point value
    /// then throws DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// \param tagId    the tag's id
    /// \param elementNumber the element number within the buffer
    /// \return the tag's value as a 64 bit floating point
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double getDouble(const TagId& tagId, size_t elementNumber) const;

    /// \brief Retrieve a tag's value as a 64 bit floating point.
    ///
    /// If the tag's value cannot be converted to a floating point value
    ///  then throws DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then returns the default value
    /// specified in the parameter.
    ///
    /// \param tagId         the tag's id
    /// \param elementNumber the element number within the buffer
    /// \param defaultValue  the value to return if the tag doesn't exist
    /// \return the tag's value as a 64 bit floating point, or defaultValue if
    ///         the tag doesn't exist
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double getDouble(const TagId& tagId, size_t elementNumber, double defaultValue) const;

    /// \brief Write a 64 bit floating point value into the element 0 of the
    ///        specified Tag's buffer 0.
    ///
    /// If the specified Tag or buffer don't exist then a new tag
    ///  is created using the specified data type (VR).
    ///
    /// \param tagId    the tag's id
    /// \param newValue the value to write into the tag
    /// \param tagVR    the tag's type to use when a new tag is created.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setDouble(const TagId& tagId, double newValue, tagVR_t tagVR);

    /// \brief Write a 64 bit floating point value into the element 0 of the
    ///        specified Tag's buffer 0.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// \param tagId    the tag's id
    /// \param newValue the value to write into the tag
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setDouble(const TagId& tagId, double newValue);

    /// \brief Retrieve a tag's value as a string.
    ///
    /// If the tag's value cannot be converted to a string then throws
    ///  DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// \param tagId    the tag's id
    /// \param elementNumber the element number within the buffer
    /// \return the tag's value as a string
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getString(const TagId& tagId, size_t elementNumber) const;

    /// \brief Retrieve a tag's value as a string.
    ///
    /// If the tag's value cannot be converted to a string then throws
    ///  DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then returns the default value
    /// specified in the parameter.
    ///
    /// \param tagId         the tag's id
    /// \param elementNumber the element number within the buffer
    /// \param defaultValue  the value to return if the tag doesn't exist
    /// \return the tag's value as a string, or defaultValue if
    ///         the tag doesn't exist
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getString(const TagId& tagId, size_t elementNumber, const std::string& defaultValue) const;

    /// \brief Write a string value into the element 0 of the specified Tag's
    ///        buffer 0.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  specific VR.
    ///
    /// \param tagId     the tag's id
    /// \param newString the string to write into the tag
    /// \param tagVR     the tag's type to use when a new tag is created.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setString(const TagId& tagId, const std::string& newString, tagVR_t tagVR);

    /// \brief Write a string value into the element 0 of the specified Tag's
    ///        buffer 0.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// \param tagId     the tag's id
    /// \param newString the string to write into the tag
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setString(const TagId& tagId, const std::string& newString);

    /// \brief Retrieve a tag's value as an Unicode string.
    ///
    /// If the tag's value cannot be converted to a Unicode string
    /// then throws DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// \param tagId    the tag's id
    /// \param elementNumber the element number within the buffer
    /// \return the tag's value as an unicode string
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::wstring getUnicodeString(const TagId& tagId, size_t elementNumber) const;

    /// \brief Retrieve a tag's value as an unicode string.
    ///
    /// If the tag's value cannot be converted to a Unicode string then throws
    ///  DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then returns the default value
    /// specified in the parameter.
    ///
    /// \param tagId         the tag's id
    /// \param elementNumber the element number within the buffer
    /// \param defaultValue  the value to return if the tag doesn't exist
    /// \return the tag's value as an unicode string, or defaultValue if
    ///         the tag doesn't exist
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::wstring getUnicodeString(const TagId& tagId, size_t elementNumber, const std::wstring& defaultValue) const;

    /// \brief Write an unicode string value into the element 0 of the specified
    ///        Tag's buffer 0.
    ///
    /// If the specified Tag doesn't exist then a new tag is created using
    /// the specified VR.
    ///
    /// \param tagId     the tag's id
    /// \param newString the string to write into the tag
    /// \param tagVR     the tag's type to use when a new tag is created.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setUnicodeString(const TagId& tagId, const std::wstring& newString, tagVR_t tagVR);

    /// \brief Write an unicode string value into the element 0 of the specified
    ///        Tag's buffer 0.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// \param tagId     the tag's id
    /// \param newString the string to write into the tag
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setUnicodeString(const TagId& tagId, const std::wstring& newString);

    /// \brief Retrieve a tag's value as Age.
    ///
    /// If the tag's value cannot be converted to Age then throws
    ///  DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// \param tagId    the tag's id
    /// \param elementNumber the element number within the buffer
    /// \return the tag's value as Age
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Age* getAge(const TagId& tagId, size_t elementNumber) const;

    /// \brief Retrieve a tag's value as Age.
    ///
    /// If the tag's value cannot be converted to Age then throws
    ///  DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then returns the default value
    /// specified in the parameter.
    ///
    /// \param tagId         the tag's id
    /// \param elementNumber the element number within the buffer 0
    /// \param defaultValue  the value to return if the tag doesn't exist
    /// \return the tag's value as Age, or defaultValue if the tag doesn't exist
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Age* getAge(const TagId& tagId, size_t elementNumber, const Age& defaultValue) const;

    /// \brief Write an Age string into the element 0 of the specified
    ///        Tag's buffer 0.
    ///
    /// If the specified Tag doesn't exist then a new tag and is created using
    /// the VR tagVR_t::AS.
    ///
    /// \param tagId    the tag's id
    /// \param age      the Age to write into the tag
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setAge(const TagId& tagId, const Age& age);

    /// \brief Retrieve a tag's value as a Date.
    ///
    /// If the tag's value cannot be converted to a Date then throws
    ///  DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then throws MissingTagError or
    ///  MissingGroupError.
    ///
    /// If the specified Tag does not exist then throws MissingItemError.
    ///
    /// \param tagId    the tag's id
    /// \param elementNumber the element number within the buffer 0
    /// \return the tag's value as a Date
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Date* getDate(const TagId& tagId, size_t elementNumber) const;

    /// \brief Retrieve a tag's value as a Date.
    ///
    /// If the tag's value cannot be converted to a date then throws
    ///  DataHandlerConversionError.
    ///
    /// If the specified Tag does not exist then returns the default value
    /// specified in the parameter.
    ///
    /// \param tagId         the tag's id
    /// \param elementNumber the element number within the buffer
    /// \param defaultValue  the value to return if the tag doesn't exist
    /// \return the tag's value as a Date, or defaultValue if the tag doesn't exist
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Date* getDate(const TagId& tagId, size_t elementNumber, const Date& defaultValue) const;

    /// \brief Write a Date string into the element 0 of the specified
    ///        Tag's buffer 0.
    ///
    /// If the specified Tag or buffer don't exist then a new tag and/or buffer
    ///  are created using the specified data type (VR).
    ///
    /// \param tagId    the tag's id
    /// \param date     the Date to write into the tag
    /// \param tagVR    the tag's type to use when a new tag is created.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setDate(const TagId& tagId, const Date& date, tagVR_t tagVR);

    /// \brief Write a Date string into the element 0 of the specified
    ///        Tag's buffer 0.
    ///
    /// If the specified Tag does not exist then it creates a new tag with a
    ///  default VR retrieved from the DicomDictionary.
    ///
    /// \param tagId    the tag's id
    /// \param date     the Date to write into the tag
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setDate(const TagId& tagId, const Date& date);

    /// \brief Return the 2 chars data type (VR) of the specified tag.
    ///
    /// \param tagId the id of the tag
    /// \return the tag's data type (VR)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    tagVR_t getDataType(const TagId& tagId) const;

#ifndef SWIG
protected:
    std::shared_ptr<imebra::implementation::dataSet> m_pDataSet;
#endif
};

}

#endif // !defined(imebraDataSet__INCLUDED_)
