/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file tag.h
    \brief Declaration of the class Tag.

*/

#if !defined(imebraLut__INCLUDED_)
#define imebraLut__INCLUDED_

#include <string>
#include <cstdint>
#include "readingDataHandlerNumeric.h"
#include "definitions.h"

#ifndef SWIG

namespace imebra
{
namespace implementation
{
class lut;
}
}

#endif

namespace imebra
{

///
/// \brief This class represents Lookup Table.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API LUT
{
    LUT(const LUT&) = delete;
    LUT& operator=(const LUT&) = delete;

#ifndef SWIG
    friend class DataSet;
    friend class VOILUT;
private:
    explicit LUT(std::shared_ptr<imebra::implementation::lut> pLut);
#endif

public:
    virtual ~LUT();

    /// \brief Returns the LUT's description.
    ///
    /// \return the LUT's description
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::wstring getDescription() const;

    ReadingDataHandlerNumeric* getReadingDataHandler() const;

    /// \brief Return the number of bits used to store a LUT value.
    ///
    /// @return the number of bits used to store a mapped value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t getBits() const;

    /// \brief Return the lut's size (the number of stored values).
    ///
    /// @return the number of mapped values stored in the lut
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t getSize() const;

    std::int32_t getFirstMapped() const;

    std::uint32_t getMappedValue(std::int32_t index) const;


#ifndef SWIG
protected:
    std::shared_ptr<imebra::implementation::lut> m_pLut;
#endif
};

}

#endif // !defined(imebraLut__INCLUDED_)
