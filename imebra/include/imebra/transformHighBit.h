/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file transformHighBit.h
    \brief Declaration of TransformHighBit.

*/

#if !defined(imebraTransformHighBit__INCLUDED_)
#define imebraTransformHighBit__INCLUDED_

#ifndef SWIG

#include <memory>
#include "definitions.h"
#include "transform.h"

namespace imebra
{
namespace implementation
{
namespace transforms
{
class transformHighBit;
}
}
}

#endif

namespace imebra
{

///
/// \brief The TransformHighBit transform shifts the input image's values
///        so they match the high-bit settings of the Transform output image.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API TransformHighBit: public Transform
{
    TransformHighBit(const TransformHighBit&) = delete;
    TransformHighBit& operator=(const TransformHighBit&) = delete;

public:
    /// \brief Constructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    TransformHighBit();

    virtual ~TransformHighBit();

};

}

#endif // imebraTransformHighBit__INCLUDED_
