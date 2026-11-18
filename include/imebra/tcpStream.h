/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file tcpStream.h
    \brief Declaration of the TCPStream class.

*/

#if !defined(tcpStream__INCLUDED_)
#define tcpStream__INCLUDED_

#include <string>
#include "baseStreamInput.h"
#include "baseStreamOutput.h"
#include "definitions.h"

namespace imebra
{

namespace implementation
{
class tcpSequenceStream;
}

class TCPActiveAddress;
class TCPAddress;

///
/// \brief Represents a TCP stream.
///
///////////////////////////////////////////////////////////////////////////////
class IMEBRA_API TCPStream: public BaseStreamInput, public BaseStreamOutput
{
    TCPStream(const TCPStream&) = delete;
    TCPStream& operator=(const TCPStream&) = delete;

#ifndef SWIG
    friend class TCPListener;
private:
    explicit TCPStream(std::shared_ptr<imebra::implementation::tcpSequenceStream> pTcpStream);
#endif

public:
    ///
    /// \brief Construct a TCP socket and connects it to the destination address.
    ///
    /// This is a non-blocking operation (the connection proceed after the
    /// constructor returns). Connection errors will be reported later while
    /// the communication happens.
    ///
    /// \param address the address to which the socket has to be connected.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit TCPStream(const TCPActiveAddress& address);

    ///
    /// \brief Returns the address of the connected peer.
    ///
    /// \return the address of the connected peer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    TCPAddress* getPeerAddress() const;

    ///
    /// \brief Instruct any pending operation to terminate.
    ///
    /// Current and subsequent read and write operations will fail by throwing
    /// the exception StreamClosedError.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void terminate();
};

}
#endif // !defined(tcpStream__INCLUDED_)
