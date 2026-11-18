/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement 
 that your application must also be GPL), you may purchase a commercial 
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file dicomDir.h
    \brief Declaration of the class DicomDirEntry
        structure (DicomDir and DirectoryRecord)

*/

#if !defined(imebraDicomDirEntry__INCLUDED_)
#define imebraDicomDirEntry__INCLUDED_

#include <memory>
#include <string>
#include "definitions.h"

#ifndef SWIG

namespace imebra
{
namespace implementation
{
class directoryRecord;
}
}

#endif

namespace imebra
{

class DataSet;

///
/// \brief Represents a single DICOMDIR entry.
///
/// Each entry can be followed by a sibling entry (on the same depth level)
/// or can point to its first child entry (one level deeper).
///
/// Each DicomDirEntry object manages a DataSet which is used to store the
/// entry's data. The DataSet objects managed by DicomDirEntry objects are
/// inserted as sequence items into the DicomDir's DataSet.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API DicomDirEntry
{
    DicomDirEntry(const DicomDirEntry&) = delete;
    DicomDirEntry& operator=(const DicomDirEntry&) = delete;

#ifndef SWIG
	friend class DicomDir;
private:
    explicit DicomDirEntry(std::shared_ptr<imebra::implementation::directoryRecord> pDirectoryRecord);
#endif

public:

    virtual ~DicomDirEntry();

    /// \brief Return the DataSet managed by the DicomDirEntry object.
    ///
    /// \return the DataSet managed by the DicomDirEntry object
    /// 
    ///////////////////////////////////////////////////////////////////////////////
    DataSet* getEntryDataSet();

    /// \brief Return the next sibling entry (on the same depth level).
    /// 
    /// If the next DicomDirEntry is missing (this is the last entry) then
    /// return a null pointer.
    ///
    /// \return the next sibling DicomDirEntry object, or a null pointer if the
    ///         next sibling DicomDirEntry does not exist
    /// 
    ///////////////////////////////////////////////////////////////////////////////
    DicomDirEntry* getNextEntry();

    /// \brief Return the first child entry (one level deeper).
    /// 
    /// If the first child DicomDirEntry is missing then return a null pointer.
    ///
    /// \return the first child DicomDirEntry object, or a null pointer if there
    ///         aren't any children
    /// 
    ///////////////////////////////////////////////////////////////////////////////
    DicomDirEntry* getFirstChildEntry();
	
    /// \brief Set the next sibling entry (one the same depth level).
    /// 
    /// \param nextEntry the next sibling DicomDirEntry object
    /// 
    ///////////////////////////////////////////////////////////////////////////////
    void setNextEntry(const DicomDirEntry& nextEntry);

    /// \brief Set the first child entry (one level deeper).
    /// 
    /// \param firstChildEntry the first child DicomDirEntry object
    /// 
    ///////////////////////////////////////////////////////////////////////////////
    void setFirstChildEntry(const DicomDirEntry& firstChildEntry);

    /// \brief Returns the parts that form the name of the file referenced by the
    ///        DicomDirEntry object.
    /// 
    /// \return a list of strings that form the name of the file referenced by the
    ///         DicomDirEntry object. The last item in the list is the file name,
    ///         while the preceding items contain the folders names.
    ///         For instance, on Linux the parts "folder0", "folder1", "fileName"
    ///         represent the path "folder0/folder1/fileName"
    /// 
    ///////////////////////////////////////////////////////////////////////////////
    fileParts_t getFileParts() const;

    /// \brief Set the parts that form the name of the file referenced by the
    ///        DicomDirEntry object.
    /// 
    /// \param fileParts  a list of strings that form the name of the file 
    ///                   referenced by the DicomDirEntry object. The last item 
    ///                   in the list is the file name, while the preceding items 
    ///                   contain the folders names.
    ///                   For instance, on Linux the parts "folder0", "folder1", 
    ///                   "fileName" represent the path "folder0/folder1/fileName"
    /// 
    ///////////////////////////////////////////////////////////////////////////////
    void setFileParts(const fileParts_t& fileParts);

    /// \brief 
    directoryRecordType_t getType() const;

    std::string getTypeString() const;

#ifndef SWIG
protected:
    std::shared_ptr<imebra::implementation::directoryRecord> m_pDirectoryRecord;
#endif
};

}

#endif // !defined(imebraDicomDirEntry__INCLUDED_)
