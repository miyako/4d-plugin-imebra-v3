/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file image.h
    \brief Declaration of the class Image.

*/

#if !defined(imebraImage__INCLUDED_)
#define imebraImage__INCLUDED_

#ifndef SWIG

#include <memory>
#include <string>
#include "definitions.h"
#include "readingDataHandlerNumeric.h"
#include "writingDataHandlerNumeric.h"

namespace imebra
{
namespace implementation
{
class image;
}
}
#endif

namespace imebra
{

class CodecFactory;
class Transform;
class VOILUT;
class DataSet;
class DrawBitmap;


///
/// \brief Represents a DICOM image.
///
/// The class manages an uncompressed DICOM image. Images are compressed
/// when they are inserted into the DataSet via DataSet::setImage() and are
/// decompressed when they are referenced by the Image class.
///
/// The image is stored in a contiguous area of memory: each channel's value
/// can occupy 1, 2 or 4 bytes, according to the Image's data type.
///
/// Channels' values are always interleaved in the Image class, regardless
/// of how they are stored in the DataSet object.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API Image
{
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

#ifndef SWIG
    friend class CodecFactory;
    friend class Transform;
    friend class VOILUT;
    friend class DataSet;
    friend class DrawBitmap;

private:
    explicit Image(std::shared_ptr<imebra::implementation::image> pImage);
#endif

public:

    /// \brief Constructor.
    ///
    /// The memory for the image is not allocated by the constructor but only when
    /// a WritingDataHandler is requested with getWritingDataHandler().
    ///
    /// \param width      the image width, in pixels
    /// \param height     the image height, in pixels
    /// \param depth      the channel values data types
    /// \param colorSpace the Image's color space
    /// \param highBit    the highest bit occupied by the channels' values
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit Image(
        std::uint32_t width,
        std::uint32_t height,
        bitDepth_t depth,
        const std::string& colorSpace,
        std::uint32_t highBit);

    /// \brief Destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~Image();

    /// \brief Retrieve the image's width, in millimiters
    ///
    /// \return the image's width, in millimiters
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double getWidthMm() const;

    /// \brief Retrieve the image's height, in millimiters
    ///
    /// \return the image's height, in millimiters
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double getHeightMm() const;

    /// \brief Set the image's size, in millimiters.
    ///
    /// \param width   the image's width, in millimiters
    /// \param height  the image's height, in millimiters
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSizeMm(double width, double height);

    /// \brief Retrieve the image's width, in pixels.
    ///
    /// \return the image's width, in pixels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::uint32_t getWidth() const;

    /// \brief Retrieve the image's height, in pixels.
    ///
    /// \return the image's height, in pixels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::uint32_t getHeight() const;

    /// \brief Retrieve a ReadingDataHandlerNumeric object referencing the Image's
    ///        memory (read only).
    ///
    /// The memory referenced by ReadingDataHandlerNumeric contains all the image's
    /// values. The color channels are interleaved.
    ///
    /// \return a ReadingDataHandlerNumeric object referencing the Image's memory
    ///         in read-only mode
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ReadingDataHandlerNumeric* getReadingDataHandler() const;

    /// \brief Retrieve a WritingDataHandlerNumeric object referencing the Image's
    ///        memory (mutable).
    ///
    /// The memory referenced by the WritingDataHandlerNumeric object is
    /// uninitialized.
    ///
    /// When the WritingDataHandlerNumeric is destroyed then the memory managed
    /// by the WritingDataHandlerNumeric object replaces the old Image's memory.
    ///
    /// \return a WritingDataHandlerNumeric object referencing an uninitialized
    ///         memory buffer that the client has to fill the the image's data
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WritingDataHandlerNumeric* getWritingDataHandler();

    /// \brief Retrieve the Image's color space
    ///
    /// \return the Image's color space
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getColorSpace() const;

    /// \brief Return the number of color channels contained by the image
    ///
    /// \return the number of color channel contained by the image
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::uint32_t getChannelsNumber() const;

    /// \brief Return the type of the channels' values.
    ///
    /// \return the type of the channels' values
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bitDepth_t getDepth() const;

    /// \brief Return the highest bit occupied by the channels' values.
    ///
    /// \return the highest bit occupied by the channels' values
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::uint32_t getHighBit() const;

#ifndef SWIG
protected:
    std::shared_ptr<implementation::image> m_pImage;
#endif
};

}

#endif // !defined(imebraImage__INCLUDED_)
