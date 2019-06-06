/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file transform.h
    \brief Declaration of the base class for transforms.

*/

#if !defined(imebraTransform__INCLUDED_)
#define imebraTransform__INCLUDED_

#include <memory>
#include "definitions.h"

#ifndef SWIG

namespace imebra
{
namespace implementation
{
namespace transforms
{
class transform;
}
}
}
#endif

namespace imebra
{

class Image;

///
/// \brief Represents a transform: a transform takes one Image as input,
///        processes it and outputs the processed Image.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API Transform
{
    Transform(const Transform&) = delete;
    Transform& operator=(const Transform&) = delete;

#ifndef SWIG
    friend class TransformsChain;
    friend class DrawBitmap;
    friend class ColorTransformsFactory;
    friend class TransformHighBit;
    friend class ModalityVOILUT;
    friend class VOILUT;

private:
    explicit Transform(std::shared_ptr<imebra::implementation::transforms::transform> pTransform);
#endif


public:

    virtual ~Transform();

    /// \brief Returns true if the transform doesn't perform any processing
    ///        (the output image will be identical to the input one).
    ///
    /// \return true if the Transform doesn't perform any processing
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool isEmpty() const;

    /// \brief Allocate an image ready to accomodate the result of the
    ///        image processing performed by the runTransform() method.
    ///
    /// \param inputImage the image that will be set as the input of the
    ///                   runTransform() method
    /// \param width      the width of the output image
    /// \param height     the height of the output image
    /// \return an image ready to be passed as outputImage to runTransform()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Image* allocateOutputImage(const Image& inputImage, std::uint32_t width, std::uint32_t height) const;

    /// \brief Run the processing function of the Transform.
    ///
    /// \param inputImage the image to use as input
    ///
	void runTransform(
            const Image& inputImage,
            std::uint32_t inputTopLeftX, std::uint32_t inputTopLeftY, std::uint32_t inputWidth, std::uint32_t inputHeight,
            Image& outputImage,
            std::uint32_t outputTopLeftX, std::uint32_t outputTopLeftY) const;

#ifndef SWIG
protected:
    std::shared_ptr<imebra::implementation::transforms::transform> m_pTransform;
#endif
};

}

#endif // imebraTransform__INCLUDED_
