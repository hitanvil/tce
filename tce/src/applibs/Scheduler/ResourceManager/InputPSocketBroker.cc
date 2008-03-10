/**
 * @file InputPSocketBroker.cc
 *
 * Implementation of InputPSocketBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "InputPSocketBroker.hh"
#include "InputPSocketResource.hh"
#include "ResourceMapper.hh"
#include "Machine.hh"
#include "Segment.hh"
#include "FUPort.hh"
#include "Terminal.hh"
#include "UniversalMachine.hh"
#include "MapTools.hh"
#include "Move.hh"
#include "MoveNode.hh"

using std::string;

using namespace TTAMachine;
using namespace TTAProgram;

/**
 * Constructor.
 */
InputPSocketBroker::InputPSocketBroker(std::string name): 
    ResourceBroker(name) {
}

/**
 * Destructor.
 */
InputPSocketBroker::~InputPSocketBroker(){
}

/**
 * Return all resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return All resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 */
SchedulingResourceSet
InputPSocketBroker::allAvailableResources(
    int cycle,
    const MoveNode& node) const {

    if (!isApplicable(node)) {
        string msg = "Broker not capable of assigning resources to node!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    Move& move = const_cast<MoveNode&>(node).move();

    SchedulingResourceSet resourceSet;

    if (move.destination().isFUPort()) {
        // psocket already implicitly assigned by fubroker
        Socket& inputSocket = *move.destination().port().inputSocket();
        if (resourceOf(inputSocket).canAssign(cycle, node)) {
            resourceSet.insert(resourceOf(inputSocket));
        }
        return resourceSet;

    } else {
        // assign psocket for writing rf
        // ImmediateUnits can not be writen to this way

        const RegisterFile& rf = move.destination().registerFile();

        for (int i = 0; i < rf.portCount(); i++) {
            Port& port = *rf.port(i);
            Socket* inputSocket = port.inputSocket();
            if (inputSocket != NULL &&
                resourceOf(*inputSocket).canAssign(cycle, node)) {
                resourceSet.insert(resourceOf(*inputSocket));
            }
        }
        return resourceSet;
    }
}

/**
 * Return the earliest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return The earliest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 */
int
InputPSocketBroker::earliestCycle(int, const MoveNode&) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return the latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return The latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 */
int
InputPSocketBroker::latestCycle(int, const MoveNode&) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return true if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return True if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 */
bool
InputPSocketBroker::isAlreadyAssigned(int cycle, const MoveNode& node) const{
    Terminal& dst = const_cast<MoveNode&>(node).move().destination();
    const Port& port = dst.port();
    if (hasResourceOf(*port.inputSocket()) &&
        resourceOf(*port.inputSocket()).isInUse(cycle) &&
        MapTools::containsKey(assignedResources_, &node)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Return true if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 *
 * This broker is applicable for input FU/RF ports.
 *
 * @param node Node.
 * @return True if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 */
bool
InputPSocketBroker::isApplicable(const MoveNode& node) const {
    Move& move = const_cast<MoveNode&>(node).move();
    return (move.destination().isFUPort() || move.destination().isGPR());
}

/**
 * Mark given resource as in use for the given node, and assign the
 * corresponding machine part (if applicable) to the node's move.
 *
 * If the node is already assigned to given resource, this method does
 * nothing.
 *
 * @param cycle Cycle.
 * @param node Node to assign.
 * @param res Resource representing input PSocket
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidParameters If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 * @todo Add caching of assigned resource objects (a MoveNode-Resource
 * mapping).
 */
void
InputPSocketBroker::assign(int cycle, MoveNode& node, SchedulingResource& res)
    throw (Exception) {

    if (!isApplicable(node)) {
        string msg = "Broker not capable of assigning resources to node!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    if (!hasResource(res)) {
        string msg = "Broker does not contain given resource.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    Move& move = node.move();

    if (move.destination().isGPR()) {

        Port* port = NULL;

        const Socket& socket = dynamic_cast<const Socket&>(machinePartOf(res));
        for (int i = 0; i < socket.portCount(); i++) {
            if (socket.port(i)->inputSocket() == &socket) {
                if (socket.port(i)->parentUnit() !=
                    &move.destination().registerFile())
                    continue;

                port = socket.port(i);
                break;
            }
        }
        if (port == NULL){
            throw InvalidData(
                __FILE__, __LINE__, __func__, 
                "Broker does not have necessary Port registered!");                               
        }        
        TerminalRegister* newDst =
            new TerminalRegister(*port, move.destination().index());
        move.setDestination(newDst);
    }

    res.assign(cycle, node);
    assignedResources_.insert(
        std::pair<const MoveNode*, SchedulingResource*>(&node,&res));
}

/**
 * Free the resource type managed by this broker and unassign it from
 * given node.
 *
 * If this broker is not applicable to the given node, or the node is
 * not assigned a resource of the managed type, this method does
 * nothing.
 *
 * @param node Node.
 * @todo Remove from cache of assigned resource objects.
 */
void
InputPSocketBroker::unassign(MoveNode& node) {
    if (!isApplicable(node)) {
        return;
    }

    if (MapTools::containsKey(assignedResources_, &node)) {
        Move& move = const_cast<MoveNode&>(node).move();
        Terminal& dst = move.destination();
        SchedulingResource& res = resourceOf(*dst.port().inputSocket());
        if ((MapTools::valueForKey<SchedulingResource*>(
            assignedResources_, &node)) != &res) {
            std::string msg =
                "InputPSocketBroker: assigned resource changed!";
            throw ModuleRunTimeError(__FILE__,__LINE__, __func__, msg);
        }
        res.unassign(node.cycle(), node);
        assignedResources_.erase(&node);
    }
}

/**
 * Build all resource objects of the controlled type required to model
 * scheduling resources of the given target processor.
 *
 * This method cannot set up the resource links (dependent and related
 * resources) of the constructed resource objects.
 *
 * @param target Target machine.
 */
void
InputPSocketBroker::buildResources(const TTAMachine::Machine& target) {
    Machine::SocketNavigator navi = target.socketNavigator();
    for (int i = 0; i < navi.count(); i++) {
        Socket* socket = navi.item(i);
        if (socket->direction() == Socket::INPUT) {
            InputPSocketResource* ipsResource =
                new InputPSocketResource(socket->name());
            ResourceBroker::addResource(*socket, ipsResource);
        }
    }
}

/**
 * Complete resource initialisation by creating the references to
 * other resources due to a dependency or a relation.
 *
 * Use the given resource mapper to lookup dependent and related resources
 * using machine parts as keys.
 *
 * @param mapper Resource mapper.
 */
void
InputPSocketBroker::setupResourceLinks(const ResourceMapper& mapper) {

    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const Socket* socket =
            dynamic_cast<const Socket*>((*resIter).first);
        if (socket == NULL){
            throw InvalidData(
                __FILE__, __LINE__, __func__, 
                "Broker does not have necessary Socket registered!");                               
        }                    
        SchedulingResource* socketResource = (*resIter).second;

        for (int i = 0; i < socket->portCount(); i++) {
            Port* port = socket->port(i);
            FunctionUnit* fu = dynamic_cast<FunctionUnit*>(port->parentUnit());

            if (fu != NULL) {
                for (int j = 0; j < mapper.resourceCount(*fu); j++) {
                    SchedulingResource& relRes = mapper.resourceOf(*fu, j);
                    if (relRes.isInputFUResource()) {
                        socketResource->addToRelatedGroup(0, relRes);
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < socket->segmentCount(); i++) {
            Segment* segment = socket->segment(i);
            try {
                SchedulingResource& relRes = mapper.resourceOf(*segment);
                socketResource->addToRelatedGroup(1, relRes);
            } catch (const KeyNotFound& e) {
                std::string msg = "InputPSocketBroker: finding ";
                msg += " resource for Segment ";
                msg += " failed with error: ";
                msg += e.errorMessageStack();
                throw KeyNotFound(
                    __FILE__, __LINE__, __func__, msg);
            }                                                        
        }
    }
}
