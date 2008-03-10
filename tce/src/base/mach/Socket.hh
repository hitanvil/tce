/**
 * @file Socket.hh
 *
 * Declaration of Socket class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 * @note reviewed 22 Jun 2004 by ao, ml, vpj, ll
 */

#ifndef TTA_SOCKET_HH
#define TTA_SOCKET_HH

#include <string>
#include <vector>
#include <set>

#include "MachinePart.hh"

namespace TTAMachine {

class Segment;
class Connection;
class Port;
class Bus;

/**
 * Represents a socket in the TTA processor.
 */
class Socket : public Component {
public:
    /**
     * Direction of data movements in socket.
     */
    enum Direction {
        INPUT,  ///< Data goes from bus to port.
        OUTPUT, ///< Data goes from port to bus.
        UNKNOWN ///< Unknown direction.
    };

    Socket(const std::string& name)
        throw (InvalidName);
    Socket(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ~Socket();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    void setDirection(Direction direction)
        throw (IllegalConnectivity);
    Direction direction() const;
    void attachBus(Segment& bus)
        throw (IllegalRegistration, IllegalConnectivity);
    void detachBus(Segment& bus)
        throw (InstanceNotFound);
    void detachBus(Bus& bus);
    int portCount() const;
    Port* port(int index) const
        throw (OutOfRange);

    void detachAllPorts();
    const Connection& connection(const Segment& bus) const;
    bool isConnectedTo(const Bus& bus) const;
    bool isConnectedTo(const Segment& bus) const;

    int segmentCount() const;
    Segment* segment(int index) const
        throw (OutOfRange);

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for socket.
    static const std::string OSNAME_SOCKET;
    /// ObjectState attribute key for socket direction.
    static const std::string OSKEY_DIRECTION;
    /// ObjectState attribute value for input direction.
    static const std::string OSVALUE_INPUT;
    /// ObjectState attribute value for output direction.
    static const std::string OSVALUE_OUTPUT;
    /// ObjectState attribute value for unknown direction.
    static const std::string OSVALUE_UNKNOWN;

private:
    /// Table of Connection pointers.
    typedef std::vector<const Connection*> ConnectionTable;
    /// Table of port connections.
    typedef std::vector<Port*> PortTable;

    /// Copying forbidden.
    Socket(const Socket&);
    /// Assingment forbidden.
    Socket& operator=(const Socket&);

    void removeConnection(const Connection* connection);
    void attachPort(Port& port);
    void detachPort(const Port& port);
    void detachAllBuses();

    /// Direction of the socket.
    Direction direction_;
    /// Contains all connections to busses.
    ConnectionTable busses_;
    /// Contains all connections to ports.
    PortTable ports_;

    // port must be able to insert and remove ports from PortTable
    friend class Port;
};
}

#include "Socket.icc"

#endif
