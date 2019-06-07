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
    \brief Declaration of the class DicomDir.

*/

#if !defined(imebraDicomDir__INCLUDED_)
#define imebraDicomDir__INCLUDED_

#include <memory>
#include <string>
#include "definitions.h"

#ifndef SWIG

namespace imebra
{

namespace implementation
{
class dicomDir;
}
}

#endif

namespace imebra
{

class DataSet;
class DicomDirEntry;

///
/// \brief Represents a DICOMDIR structure.
///
/// In order to work DicomDir needs a DataSet, which embeds the DicomDir's
/// entries.
///
/// If the DicomDir's content is modified then the method updateDataSet()
/// has to be called before the managed DataSet can be stored as a DICOMDIR
/// file.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API DicomDir
{
    DicomDir(const DicomDir&) = delete;
    DicomDir& operator=(const DicomDir&) = delete;

public:
    /// \brief Construct an empty DicomDir.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    DicomDir();

    /// \brief Construct a DicomDir from a DataSet object.
    ///
    /// \param fromDataSet the DataSet created from reading a DICOMDIR file
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit DicomDir(const DataSet& fromDataSet);

    /// \brief Destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~DicomDir();

    /// \brief Return a new DicomDirEntry record and insert it into the DicomDir's
    ///        DataSet.
    ///
    /// \param recordType the type of the new entry
    /// \return a new DicomDirEntry object that can be inserted into the DicomDir
    ///         object or one of its children DicomDirEntry entries.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    DicomDirEntry* getNewEntry(directoryRecordType_t recordType);

    /// \brief Retrieve the first DicomDir's root entry.
    ///
    /// If the root entry is missing then return a null pointer.
    ///
    /// \return the root DicomDirEntry or a null pointer if the root
    ///         DicomDirEntry does noot exist
    ///
    ///////////////////////////////////////////////////////////////////////////////
    DicomDirEntry* getFirstRootEntry() const;

    /// \brief Set the specified entry as the first DicomDir's root record.
    ///
    /// After the DicomDir's content has been modified it is necessary to call
    /// updateDataSet() before the DataSet managed by DicomDir can be used.
    ///
    /// \param firstEntryRecord the DicomDirEntry object to set as the first root
    ///                         entry
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setFirstRootEntry(const DicomDirEntry& firstEntryRecord);

    /// \brief This method has to be called to update the managed DataSet after
    ///        the DicomDir's content has been updated.
    ///
    /// It is not necessary to call updateDataSet() after every DicomDir update,
    /// just call it when the updated DataSet object is needed.
    ///
    /// \return the updated managed DataSet object
    ///
    ///////////////////////////////////////////////////////////////////////////////
    DataSet* updateDataSet();

#ifndef SWIG
protected:
    std::shared_ptr<imebra::implementation::dicomDir> m_pDicomDir;
#endif
};

}

#endif // !defined(imebraDicomDir__INCLUDED_)
