/**
 * @file MoveNode.cc
 *
 * Implementation of MoveNode class.
 *
 * Nodes are the minimum independent unit of information in a
 * minimally-ordered program representation. Typically, but not necessarily,
 * the nodes in a program representation are linked together by dependences
 * and thus form a graph.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <boost/format.hpp>

#include "MoveNode.hh"
#include "Port.hh"
#include "SpecialRegisterPort.hh"
#include "Bus.hh"
#include "UniversalMachine.hh"
#include "ProgramOperation.hh"
#include "POMDisassembler.hh"
#include "AssocTools.hh"
#include "FUPort.hh"
#include "HWOperation.hh"

using namespace TTAMachine;

/**
 * Constructor.
 *
 * Creates a new node with a reference to the given Move. The ownership of the
 * move is NOT transferred to the MoveNode.
 *
 * @param newmove the Move this node refers to.
 */

MoveNode::MoveNode(TTAProgram::Move& newmove) :
    placed_(false), cycle_(0), move_(&newmove),
    moveOwned_(false), srcOp_(NULL), dstOp_(NULL) {
}

/**
 * Constructor.
 *
 * Creates a new node with Move. The ownership of the move is transferred
 * to the MoveNode.
 *
 * @param newmove the Move this node contains.
 */
MoveNode::MoveNode(TTAProgram::Move* newmove) :
    placed_(false), cycle_(0), move_(newmove),
    moveOwned_(true), srcOp_(NULL), dstOp_(NULL) {
}


/**
 * Constructor.
 *
 * Creates a new node to be entry node.
 *
 */

MoveNode::MoveNode() :
/*    GraphNode(idCounter_++),  */
    placed_(false), cycle_(0), move_(NULL),
    moveOwned_(false), srcOp_(NULL), dstOp_(NULL) {
}

/**
 * Destructor.
 *
 * Deletes the owned Move instance.
 * Does not unregister this movenode from ProgramOperations.
 */
MoveNode::~MoveNode() {

    if (isSourceOperation()) {
        sourceOperation().removeOutputNode(*this);
    }
    if (isDestinationOperation()) {
        destinationOperation().removeInputNode(*this);
    }

    if (moveOwned_) {
        delete move_;
    }
    move_ = NULL;
}

/**
 * Creates a deep copy of MoveNode.
 *
 * Sets the source and destination operation of the copy to the same as
 * original. Does not copy the cycle.
 *
 * @return return copy of MoveNode.
 */
MoveNode*
MoveNode::copy() {

    MoveNode* newNode = NULL;
    if (move_ != NULL) {
        newNode = new MoveNode(move_->copy());
    } else {
        newNode = new MoveNode;
    }

    if (isSourceOperation()) {
        sourceOperation().addOutputNode(*newNode);
        newNode->setSourceOperation(sourceOperation());
    }

    if (isDestinationOperation()) {
        destinationOperation().addInputNode(*newNode);
        newNode->setDestinationOperation(destinationOperation());
    }
    return newNode;
}

/**
 * Tells whether the source of the MoveNode (move) belongs to an operation.
 *
 * @return True if the source of the MoveNode is an operation output.
 */
bool
MoveNode::isSourceOperation() const {
    if (move_ == NULL) {
        return false;
    }
    return srcOp_ != NULL;
}


/**
 * Tells whether the node (move) reads a program variable or, if assigned, a
 * GPR.
 *
 * @return True if the source of the node is a variable or GPR.
 */
bool
MoveNode::isSourceVariable() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->source().isGPR();
}
/**
 * Tells whether the node (move) reads a Immediate Register
 *
 * @return True if the source of the node is Immediate register.
 */
bool
MoveNode::isSourceImmediateRegister() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->source().isImmediateRegister();
}


/**
 * Tells whether the source of the node (move) is a program constant. If
 * assigned, the constant is an in-line immediate or an immediate register.
 *
 * @return True if the source of the node is a constant.
 */
bool
MoveNode::isSourceConstant() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->source().isImmediate();
}


/**
 * Tells whether the destination of the node (move) belongs to an operation.
 *
 * @return True if the destination of the node is an operation input.
 */
bool
MoveNode::isDestinationOperation() const {
    if (move_ == NULL) {
        return false;
    }
    return dstOp_ != NULL;
}

/**
 * Tells whether the move belongs to an operation execution.
 *
 * @return True if the the node belongs to an operation.
 */
bool
MoveNode::isOperationMove() const {
    return isSourceOperation() || isDestinationOperation();
}

/**
 * Tells whether the node (move) writes a program variable or, if assigned,
 * a GPR.
 *
 * @return True if the destination of the node is a variable or GPR.
 */
bool
MoveNode::isDestinationVariable() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->destination().isGPR();
}


/**
 * Tells whether the node is a ``software bypass'' - both its source and its
 * destination belong to operations.
 *
 * @return True if both source and destination of the node are operation
 *     terminals.
 */
bool
MoveNode::isBypass() const {
    return isSourceOperation() && isDestinationOperation();
}

/**
 * Tells whether the node is a register move, thus not belong to
 * any particular ProgramOperation.
 *
 * It's either an immediate move to an register or a register to register
 * copy.
 *
 * @return True if the move is a register to register copy.
 */
bool
MoveNode::isRegisterMove() const {
    return !isSourceOperation() && !isDestinationOperation();
}


/**
 * Returns true if this MoveNode is in the same ProgramOperation as the
 * given MoveNode.
 *
 * @return True if both MoveNodes belong to the same ProgramOperation.
 */
bool
MoveNode::inSameOperation(const MoveNode& other) const {

    if (other.isRegisterMove() || this->isRegisterMove())
        return false;

    // due to bypass moves we have to consider both ends of each moves
    // separately.. there has to be quicker way to check this ;-)
    std::set<ProgramOperation*> operationsA;
    std::set<ProgramOperation*> operationsB;

    if (this->isSourceOperation())
        operationsA.insert(&this->sourceOperation());

    if (this->isDestinationOperation())
        operationsA.insert(&this->destinationOperation());

    if (other.isSourceOperation())
        operationsB.insert(&other.sourceOperation());

    if (other.isDestinationOperation())
        operationsB.insert(&other.destinationOperation());

    std::set<ProgramOperation*> commonOperations;

    AssocTools::intersection(operationsA, operationsB, commonOperations);
    return commonOperations.size() > 0;
}


/**
 * Tells whether is placed in the program representation, that is, has a
 * cycle assigned to it.
 *
 * @return True if a cycle is assigned to the node.
 */
bool
MoveNode::isPlaced() const {
    return placed_;
}


/**
 * Tells whether the node is fully assigned.
 *
 * A node is fully assigned when all the resources of the target processor
 * necessary to carry out the transport it specifies are assigned to it.
 *
 * @return True if all required scheduling resources of target machine are
 *      assigned to the node.
 */
bool
MoveNode::isAssigned() const {

    if (move_ == NULL) {
        // probably a dummy ENTRYNODE or something
        return false;
    }

    UniversalMachine* uMach =
        dynamic_cast<UniversalMachine*>(move_->bus().machine());
    /// Machine found is NOT UniversalMachine - we are happy
    if (uMach == NULL) {
        if ((isSourceOperation() ||
            isSourceVariable() ||
            isSourceImmediateRegister()) &&
            (move_->source().port().parentUnit()->machine()!=
             move_->bus().machine())) {
            return false;
        }
        if ((isDestinationOperation() || isDestinationVariable()) &&
            (move_->destination().port().parentUnit()->machine()!=
             move_->bus().machine())) {
            return false;
        }
        return true;
    }
    return false;
}

/**
 * Tells whether the node is completely scheduled.
 *
 * A node is completely scheduled only if it is assigned to a program cycle
 * and the necessary resources of the target processor are assigned to it.
 *
 * @return True if the node is placed and assigned.
 */
bool
MoveNode::isScheduled() const {
    return isAssigned() && isPlaced();
}


/**
 * Returns the cycle (index) assigned to the node.
 *
 * @return The cycle in which the node is placed.
 * @exception InvalidData if the node is not placed.
 */
int
MoveNode::cycle() const
    throw(InvalidData){

    if (!isPlaced()){
        std::string msg = "MoveNode was not placed yet: ";
        if (isMove()) {
            msg+= POMDisassembler::disassemble(*move_);
        } else {
            msg+= "Node does not contain a move.";
        }
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    } else {
        return cycle_;
    }
}


/**
 * Returns the enclosing scheduling scope of the node.
 *
 * @return A scheduling scope.
 */
Scope&
MoveNode::scope(){
    ///TODO: Intentionally falsified code
    /// Class Scope does not exists so far in real
    Scope* ns = new Scope;
    return *ns;
}


/**
 * Returns the instance of operation in the program whose output is the
 * source of this node.
 *
 * @return A program operation.
 * @exception InvalidData if the given node does not read an operation
 *     output.
 */
ProgramOperation&
MoveNode::sourceOperation() const
    throw(InvalidData){
    if (!isSourceOperation()){
        std::string msg = 
            (boost::format(
                "MoveNode: '%s' source is not Operation.") % toString()).
            str();
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    } else {
        return *srcOp_;
    }
}


/**
 * Returns the instance of operation in the program whose input is the
 * destination of this node.
 *
 * @return A program operation.
 * @exception InvalidData if the given node does not write an operation
 *     input.
 */
ProgramOperation&
MoveNode::destinationOperation() const
    throw(InvalidData){

    if (!isDestinationOperation()){
        std::string msg = "MoveNode destination is not Operation.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    } else {
        return *dstOp_;
    }
}

/**
 * Set cycle for a node, also sets placed_
 * @param newcycle Cycle to which node is placed_
 * @throw InvalidData If node is already placed in cycle different from
 *                      newcycle
 */
void
MoveNode::setCycle( const int newcycle)
    throw(InvalidData){

    if (placed_ == true && cycle_ != newcycle) {
        std::string msg = "MoveNode is already placed in cycle ";
        msg += cycle_;
        msg += ".";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    cycle_ = newcycle;
    placed_ = true;
}

/**
 * Unset cycle from nodes
 * @throw InvalidData If node is not placed
 */
void
MoveNode::unsetCycle()
    throw(InvalidData){

    if (placed_ == false ) {
        std::string msg = "MoveNode is not placed.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    cycle_ = 0;
    placed_ = false;
}

/**
 * Access Move inside MoveNode, casting constantness away
 *
 * @return reference to Move inside MoveNode
 */
TTAProgram::Move&
MoveNode::move(){
    if (move_ == NULL) {
        throw NotAvailable(__FILE__,__LINE__,__func__,
            "Move of MoveNode is NULL");
    }
    return const_cast<TTAProgram::Move&> (*move_);
}

/**
 * Access Move inside MoveNode, const version.
 *
 * @return reference to Move inside MoveNode
 */
const TTAProgram::Move&
MoveNode::move() const {
    if (move_ == NULL) {
        throw NotAvailable(__FILE__,__LINE__,__func__,
            "Move of MoveNode is NULL");
    }
    return *move_;
}

/**
 * Set a destination of MoveNode to ProgramOperation
 *
 * @param po Program operation that is destination of MoveNode
 */
void MoveNode::setDestinationOperation(ProgramOperation &po) {
    dstOp_ = &po;
}
/**
 * Set a source of MoveNode to ProgramOperation
 *
 * @param po Program operation that is source of MoveNode
 */
void MoveNode::setSourceOperation(ProgramOperation &po) {
    srcOp_ = &po;
}

/**
 * Returns type of the MoveNode.
 *
 * Not yet used anywhere and types not decided so
 * current dummy implementation returns -1
 *
 * @return type of the node
 */
int
MoveNode::type() {
    return -1;
}

/**
 * Returns string with ID of the MoveNode.
 *
 * Not yet used anywhere except printing graph in .dot file. Returns the
 * disassembly of the move along with its id.
 *
 * @return The string with node ID.
 */
std::string
MoveNode::toString() const {
    if (move_ == NULL) {
        return "-1:\tENTRYNODE";
    }
    std::string content = POMDisassembler::disassemble(*move_);
    return content;
}

/**
 * Returns Dot representation of the node.
 *
 * Prints the disassembly of the move and sets the color of the node to red
 * in case it's scheduled.
 *
 * @return The string with node ID.
 */
std::string
MoveNode::dotString() const {

    std::string contents = GraphNode::dotString();
    if (isOperationMove()) {
        unsigned operationId = 0;

        // make the outline of the moves that belong to the same operation
        // of the same color to aid in schedule debugging
        if (isSourceOperation())
            operationId = sourceOperation().poId();
        else
            operationId = destinationOperation().poId();

        // hash the colors so that they are easy to separate/recognise.
        // srand();rand() pair only used as a hash function, 
        // this needs to be deterministic.
        srand(operationId);
        int operationColor = rand() / (RAND_MAX>>24);

        contents +=
            (boost::format(
                ",color=\"#%.6x\"") % operationColor).str();
    }

    if (isScheduled()) {
        contents += ",shape=box";
    } else {
        contents += ",shape=ellipse";
    }
    return contents;
}

/**
 * Returns true if it's a real move, not a pseudo move such as entry node.
 */
bool
MoveNode::isMove() const {
    return move_ != NULL;
}

/**
 * Returns the cycle the given result move can be scheduled earliest,
 * taking in the account the latency of the operation.
 *
 * In case the trigger move has not been scheduled yet, returns INT_MAX.
 *
 * @exception IllegalObject if this MoveNode is not a result read.
 */
int
MoveNode::earliestResultReadCycle() const {

    if (!isSourceOperation())
        throw IllegalParameters(
            __FILE__, __LINE__, __func__, "Not a result read move.");

    const ProgramOperation& po = sourceOperation();

    try {
        MoveNode& trigger = po.triggeringMove();
        if (!trigger.isScheduled()) {
            return INT_MAX;
        }

        // find the latency of the operation output we are reading
        const TTAMachine::HWOperation& hwop =
            *trigger.move().destination().functionUnit().operation(
                po.operation().name());

        // find the OSAL id of the operand of the output we are reading
        const int outputIndex = move_->source().operationIndex();
        return trigger.cycle() + hwop.latency(outputIndex);
    } catch (const InvalidData& id) {
        // triggeringMove() throws if the triggering move cannot be resolved
        // again ignore this. causd by either
        // incorrect scheduling order ( in RM tests) or
        // broken machine ( catched by machinecheck now)
    } catch (const Exception& e) {
        abortWithError(e.errorMessageStack());
    }
    return INT_MAX;
}

/**
 * Unsets destination operation.
 *
 * Does not ask the ProgramOperation to remove this MoveNode from
 * it's input moves.
 */
void
MoveNode::unsetDestinationOperation() {
    dstOp_ = NULL;
}

/**
 * Unsets source operation.
 *
 * Does not ask the ProgramOperation to remove this MoveNode from
 * it's output moves.
 */
void
MoveNode::unsetSourceOperation() {
    srcOp_ = NULL;
}

/**
 * Test if Move is owned by MoveNode.
 *
 * @return True if Move is owned by MoveNode and will be destroyed by it.
 */
bool
MoveNode::isMoveOwned() const {
    return moveOwned_;
}

/**
 * Sets flag to notify MoveNode that it owns it's Move and has to destroy
 * it in destructor. Happens when Move is removed from Instruction by
 * unassigning in RM.
 *
 */
void
MoveNode::setMoveOwned() {
    moveOwned_ = true;
}

/**
 * Unsets flag to notify MoveNode that it does not own it's Move. Ownership
 * was passed to Instruction object most likely during assignment by RM.
 *
 */
void
MoveNode::unsetMoveOwned() {
    moveOwned_ = false;
}

