/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file dicomDictionary.h
    \brief Declaration of the DicomDictionary class.

*/

#if !defined(imebraDicomDictionary__INCLUDED_)
#define imebraDicomDictionary__INCLUDED_

#include <string>
#include "dataSet.h"
#include "streamReader.h"
#include "streamWriter.h"
#include "definitions.h"

namespace imebra
{

class TagId;

///
/// \brief Provides information about the tag's description and their default
///        data type (VR).
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API DicomDictionary
{
public:
    /// \brief Retrieve a tag's description.
    ///
    /// @param id        the tag's id
    /// @return          The tag's description
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static std::wstring getUnicodeTagName(const TagId& id);

    /// \brief Retrieve a tag's description.
    ///
    /// @param id        the tag's id
    /// @return          The tag's description
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static std::string getTagName(const TagId& id);

    /// \brief Retrieve a tag's default data type.
    ///
    /// @param id        the tag's id
    /// @return          The tag's data type
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static tagVR_t getTagType(const TagId& id);

    /// \brief Return the size of the data type's elements
    ///
    /// @param dataType the data type for which the information is required
    /// @return the size of a single element
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static std::uint32_t getWordSize(tagVR_t dataType);

    /// \brief Return the maximum size of the tags with the specified data type.
    ///
    /// @param dataType the data type for which the information is required
    /// @return         the maximum tag's size in bytes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static std::uint32_t getMaxSize(tagVR_t dataType);
};

}
#endif // !defined(imebraDicomDictionary__INCLUDED_)
