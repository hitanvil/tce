/**
 * @file ExecutionPipelineResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the ExecutionPipelineResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ExecutionPipelineResource.hh"
#include "MapTools.hh"
#include "Move.hh"
#include "Operation.hh"
#include "Application.hh"
#include "Exception.hh"
#include "ProgramOperation.hh"
#include "MapTools.hh"
#include "MoveNode.hh"
#include "POMDisassembler.hh"
#include "OutputPSocketResource.hh"
#include "InputPSocketResource.hh"
#include "FunctionUnit.hh"
/**
 * Constructor.
 *
 * Creates new resource with defined name
 *
 * @param name Name of resource
 * @param resNum Number of resources in FU
 * @param maxLatency Latency of longest operation FU supports
 */
ExecutionPipelineResource::ExecutionPipelineResource(
    const std::string& name,
    const unsigned int maxLatency,
    const unsigned int resNum) :
    SchedulingResource(name), numberOfResources_(resNum),
    maximalLatency_(maxLatency), cachedSize_(INT_MIN) {}


/**
 * Empty destructor
 */
ExecutionPipelineResource::~ExecutionPipelineResource() {}

/**
 * Not to be used. ExecutionPipelineResource needs to be
 * tested also with PSocket parameter to find if the desired
 * part of MoveNode is source or destination from type of PSocket.
 */
bool
ExecutionPipelineResource::canAssign(const int, const MoveNode&) const {
    abortWithError("Wrong use of canAssign, use also third parameter!");
    return false;
}

/**
 * Test if resource ExecutionPipelineResource is used in given cycle.
 *
 * If there is any of pipeline resources already used in given cycle.
 * @param cycle Cycle which to test
 * @return True if ExecutionPipelineResource is already used in cycle
 * @throw Internal error, the recorded resource usage for cycle is shorter
 * then the number of resources the FU has.
 */
bool
ExecutionPipelineResource::isInUse(const int cycle) const
    throw (Exception) {
    int progOpCount = operandsWriten_.size();
    if (cycle < progOpCount && operandsWriten_.at(cycle) != NULL) {
        return true;
    }
    int resultReadCount = resultRead_.size();
    if (cycle <  resultReadCount && resultRead_.at(cycle).first != NULL) {
        return true;
    }
    if (cycle >= size()) {
        return false;
    }
    if (numberOfResources_ !=
        static_cast<int>(fuExecutionPipeline_[cycle].size())) {
        std::string msg = "Execution pipeline is missing resources!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    for (int i = 0; i < numberOfResources_; i++) {
        if (fuExecutionPipeline_[cycle][i]) {
            return true;
        }
    }
    return false;
}

/**
 * Test if resource ExecutionPipelineResource is available for any of
 * the supported operations (at least one).
 *
 * @param cycle Cycle which to test
 * @return True if ExecutionPipelineResource is available in cycle
 */
bool
ExecutionPipelineResource::isAvailable(const int cycle) const {
    int progOpCount = operandsWriten_.size();

    if (cycle >= size()) {
        // whole new operation will be in part of scope that is not
        // assigned to FU till now, so it is implicitely free
        if (cycle >= progOpCount) {
            return true;
        }
    }
    if (cycle < progOpCount) {
        // cycle is in range between 2 moves of same operation
        // we can not overwrite it!
        if (operandsWriten_.at(cycle) != NULL) {
            return false;
        }
    }
    bool foundConflict = false;
    for (unsigned int pI = 0; pI < operationPipelines_.size(); pI++) {
        foundConflict = false;
        for (int i = 0; i < maximalLatency_; i++) {
            if ((cycle + i) >= size()) {
                if (!foundConflict) {
                    return true;
                }
            }
            for (int j = 0 ; j < numberOfResources_; j++) {
                if (operationPipelines_[pI][i][j] &&
                    fuExecutionPipeline_[cycle+i][j]) {
                    foundConflict = true;
                    // One of supported operations is conflicting, still
                    // needs to test other operations.
                    break;
                }
            }
        }
        if (!foundConflict) {
            // Last tested operation was not conflicting
            return true;
        }
    }
    if (!foundConflict) {
        return true;
    }
    return false;
}

void
ExecutionPipelineResource::assign(const int, MoveNode&)
    throw (Exception) {
    abortWithError("Execution Pipeline Resource needs 3 arguments assign");
}

/**
 * Assign resource to given node for given cycle.
 *
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 */
void
ExecutionPipelineResource::assign(
    const int cycle,
    MoveNode& node,
    bool source)
    throw (Exception) {

    cachedSize_ = INT_MIN;
    
    if (node.isSourceOperation() && source) {

        assignedSourceNodes_.insert(&node);
        ProgramOperation* pOp = &node.sourceOperation();
        // record Program Operation in cycle where the result is read
        int readCount = resultRead_.size();
        if (readCount <= cycle) {
            for (int i = readCount; i <= cycle; i++) {
                resultRead_.push_back(
                    std::pair<ProgramOperation*, int>(NULL, 0));
            }
        }

        if (resultRead_.at(cycle).first == NULL) {
            resultRead_.at(cycle) =
                std::pair<ProgramOperation*, int>(pOp,1);
        } else {
            resultRead_.at(cycle).second++;
        }

        // record program Operation in cycle where result is available
        int resCount = resultWriten_.size();
        int resultReady = node.earliestResultReadCycle();
        if (resultReady == INT_MAX) {
            resultReady = cycle;
        }

        if (resCount <= resultReady) {
            for (int i = resCount; i <= resultReady; i++) {
                resultWriten_.push_back(
                    std::pair<ProgramOperation*,int>(NULL, 0));
            }
        }
        if (resultWriten_.at(resultReady).first == NULL) {
            resultWriten_.at(resultReady) =
                std::pair<ProgramOperation*, int>(&node.sourceOperation(), 1);
        } else {
            resultWriten_.at(resultReady).second++;
        }
        storedResultCycles_.insert(
            std::pair<MoveNode*, int>(&node,resultReady));
    }

    MoveNode* newNode = const_cast<MoveNode*>(&node);
    std::string opName = "";
    if (!node.isDestinationOperation() || source) {
        return;
    }

    // If move is opcode setting, we set operation based on opcode.
    // If it is only triggering, then opcode from previous opcode setting
    // move is applied....
    assignedDestinationNodes_.insert(&node);
    ProgramOperation* pOp = NULL;
    try {
        pOp = &newNode->destinationOperation();
    } catch (const InvalidData& e) {
        abortWithError(e.errorMessage());
    }
    int firstCycle = 0;
    int lastCycle = 0;
    int triggering = 0;
    int progOpCount = operandsWriten_.size();
    findRange(cycle, node, firstCycle, lastCycle, triggering);
    if (progOpCount < cycle) {
        operandsWriten_.resize(cycle, NULL);
    }
    if (unsigned(lastCycle) >= operandsWriten_.size()) {
        operandsWriten_.resize(lastCycle + 1, pOp);
    }
    for (int i = firstCycle; i <= lastCycle; i++) {
        if (operandsWriten_[i] != pOp && operandsWriten_[i] != NULL) {
            std::string msg =  name() + " had previous operation ";
            msg += operandsWriten_[i]->toString() + " in cycle " ;
            msg += Conversion::toString(i);
            msg += " " + node.toString();
            msg += "\n";
            msg += newNode->sourceOperation().toString() + " :: ";
            msg += newNode->destinationOperation().toString();
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
        operandsWriten_[i] = pOp;
    }

    if (!newNode->move().destination().isTriggering()) {
        // new destination of FU is set before this method is called
        // so we can trust that the port knowns if it is triggering
        return;
    }
    if (newNode->move().destination().isOpcodeSetting()) {
        opName = newNode->move().destination().operation().name();
    } else {
        for (int i = cycle; i >= 0; i--) {
            if (MapTools::containsKey(opcodeSettingCycle_, i)) {
                opName = MapTools::valueForKey<std::string>
                    (opcodeSettingCycle_, i);
                break;
            }
        }
        if (opName == "") {
            std::string msg = "Using non opcodeSetting triggering move";
            msg += " without setting opcode first!";
            msg += " Move: " + node.toString();
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        }
    }
    int pIndex = 0;
    if (MapTools::containsKey(operationSupported_, opName)) {
        pIndex = MapTools::valueForKey<int>(operationSupported_, opName);
    } else {
        std::string msg = "Operation " + opName ;
        msg += " is not implemented in " + name() + "!";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }

    int fuEpSize = fuExecutionPipeline_.size();
    // add empty lines until one BEFORE cycle we want to assign
    if (cycle >= fuEpSize) {
        for (int k = 0; k < cycle - fuEpSize; k++) {
            ResourceVector newEmptyLine(numberOfResources_, false);
            fuExecutionPipeline_.push_back(newEmptyLine);
        }
    }
    fuEpSize = fuExecutionPipeline_.size();
    for (int i = 0; i < maximalLatency_; i++) {
        if ((cycle + i) >= fuEpSize) {
            // cycle is beyond size() so we add line from operationPipeline
            fuExecutionPipeline_.push_back(operationPipelines_[pIndex][i]);
        } else {
            for (int j = 0 ; j < numberOfResources_; j++) {
                fuExecutionPipeline_[cycle + i][j] =
                    fuExecutionPipeline_[cycle + i][j] ||
                    operationPipelines_[pIndex][i][j];
            }
        }
    }
    // Even if the operation was just triggering not opcode setting
    // it started new execution with old opcode so we add it to the list.
    opcodeSettingCycle_.insert(std::pair<int, std::string>(cycle, opName));

}

void
ExecutionPipelineResource::unassign(const int, MoveNode&)
    throw (Exception) {
    abortWithError("Execution Pipeline Resource needs 3 arguments unassign");
}

/**
 * Unassign resource from given node for given cycle.
 *
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 * @throw In case there was no previous assignment or wrong operation
 * is unassigned
 */
void
ExecutionPipelineResource::unassign(
    const int cycle,
    MoveNode& node,
    bool source)
    throw (Exception) {

    cachedSize_ = INT_MIN;

    if (node.cycle() != cycle) {
        throw InvalidData(__FILE__, __LINE__, __func__, 
            "Trying to unassign node from different cycle "
            "then it was assigned to!");
    }
    if (node.isSourceOperation() && source) {
        if (assignedSourceNodes_.find(&node) != assignedSourceNodes_.end()) {
            assignedSourceNodes_.erase(&node);

            if (MapTools::containsKey(storedResultCycles_, &node)) {
                int resultReady =
                    MapTools::valueForKey<int>(storedResultCycles_, &node);
                // Decrease counter of results written in
                resultWriten_.at(resultReady).second--;
                if (resultWriten_.at(resultReady).second == 0) {
                    resultWriten_.at(resultReady).first = NULL;
                }
                storedResultCycles_.erase(&node);

                // Each result read is recorded when assign is called
                resultRead_.at(cycle).second--;
                // Decrease counter of result reads
                if (resultRead_.at(cycle).second == 0) {
                    resultRead_.at(cycle).first = NULL;
                }
                int maxResults = resultWriten_.size() - 1;
                while (maxResults >= 0 &&
                    resultWriten_.at(maxResults).first == NULL) {
                    maxResults--;
                    resultWriten_.pop_back();
                }

                int maxResultReads = resultRead_.size() -1;
                while (maxResultReads >= 0 &&
                    resultRead_.at(maxResultReads).first == NULL) {
                    maxResultReads--;
                    resultRead_.pop_back();
                }
            }
        }
    }
    if (!node.isDestinationOperation() || source) {
        // basicaly this catches writes to ra register
        return;
    }
    if (assignedDestinationNodes_.find(&node) ==
        assignedDestinationNodes_.end()) {
        return;
    }
    assignedDestinationNodes_.erase(&node);

    int progOpCount = operandsWriten_.size();
    if (((unsigned)cycle >= fuExecutionPipeline_.size() &&
        cycle >= progOpCount) ||
            cycle < 0) {
        std::string msg = "Trying to unassign cycle from out of scope - ";
        msg += Conversion::toString(cycle);
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    ProgramOperation* pOp = &node.destinationOperation();
    int firstCycle = 0;
    int lastCycle = 0;
    int triggering = 0;
    findRange(cycle, node, firstCycle, lastCycle, triggering);
    for (int j = 0; j < pOp->inputMoveCount(); j++) {
        if (pOp->inputMove(j).isScheduled() &&
            &pOp->inputMove(j) != &node) {
            if (pOp->inputMove(j).cycle() == firstCycle) {
                // found second operand in same cycle, keep PO for that cycle
                firstCycle++;
            }
            if (pOp->inputMove(j).cycle() == lastCycle) {
                // found second operand in same cycle
                lastCycle--;
            }
        }
    }
    if (firstCycle < progOpCount &&
        (firstCycle == cycle || lastCycle == cycle)) {
        // only remove PO if node was first or last of all inputs
        // do not make hole in between other operands of same PO!
        for (int i = lastCycle; i >= firstCycle; i--) {
            if (unsigned(i) == operandsWriten_.size() - 1) {
                operandsWriten_.at(i) = NULL;
                operandsWriten_.pop_back();
            } else {
                operandsWriten_.at(i) = NULL;
            }
        }
    }
    int maxOperandsWrite = operandsWriten_.size() -1;
    while (maxOperandsWrite >= 0 &&
        operandsWriten_.at(maxOperandsWrite) == NULL) {
        maxOperandsWrite--;
    }
    operandsWriten_.resize(maxOperandsWrite + 1);

    if (!node.move().destination().isTriggering()) {
        return;
    }
    if (!MapTools::containsKey(opcodeSettingCycle_, cycle)) {
        std::string msg = "Trying to unassing operation \"";
        msg += node.move().destination().operation().name();
        msg += "\" not assigned in cycle (";
        msg += Conversion::toString(cycle);
        msg += ")!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    std::string recordedName = "";
    recordedName =
        MapTools::valueForKey<std::string>(opcodeSettingCycle_, cycle);
    MoveNode* newNode = const_cast<MoveNode*>(&node);
    std::string opName = "";
    if (newNode->move().destination().isOpcodeSetting()) {
        opName = newNode->move().destination().operation().name();
    } else {
        opName = recordedName;
    }
    if (!MapTools::containsKey(operationSupported_, opName)) {
        std::string msg = "Trying to unassign operation \'";
        msg += opName ;
        msg += "\' not supported on FU!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    if (opName != recordedName) {
        // we have different operation started in cycle then we are
        // trying to unassign
        std::string msg = "Trying to unassign operation ";
        msg += opName;
        msg += " , different from what was recorded (";
        msg += recordedName;
        msg += "). Move: " + node.toString(); 
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    int pIndex = 0;
    pIndex = MapTools::valueForKey<int>(operationSupported_, opName);

    // can not trust size() since that one ignores empty pipeline
    // and here we need to go up to the maximalLatency
    int fuEpSize = fuExecutionPipeline_.size();
    if ((cycle + maximalLatency_ - 1) >= fuEpSize) {
        std::string msg = "Unassigning operation longer then scope!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    for (int i = 0; i < maximalLatency_; i++) {
        for (int j = 0 ; j < numberOfResources_; j++) {
            fuExecutionPipeline_[cycle + i][j] =
                fuExecutionPipeline_[cycle + i][j] &&
                (!operationPipelines_[pIndex][i][j]);
        }
    }
    opcodeSettingCycle_.erase(cycle);
}

/**
 * Return true if resource can be assigned for given node in given cycle.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
ExecutionPipelineResource::canAssign(
    const int cycle,
    const MoveNode& node,
    const PSocketResource& pSocket,
    const bool triggers) const
    throw (Exception) {

    if (node.isSourceOperation() && pSocket.isOutputPSocketResource()) {
        int resultReady = node.earliestResultReadCycle();
        if (resultReady != INT_MAX && cycle < resultReady) {
            // resultReady is INT_MAX if trigger was not scheduled yet
            // also tested cycle can not be before result is in output
            // register
            return false;
        }

        // next result cycle returns where the other
        // result is written
        int otherResult = nextResultCycle(resultReady, node);
        if (otherResult <= cycle) {
            return false;
        }

        const ProgramOperation* pOp = &node.sourceOperation();
        int maxResultRead = resultRead_.size();
        for (int i = resultReady; i < maxResultRead; i++) {
            if (i < maxResultRead) {
                if (resultRead_.at(i).second > 0) {
                    if (i < otherResult &&
                        resultRead_.at(i).first != pOp) {
                    // Other result was written earlier and is read
                    // after this one would be written in cycle
                    // we can not overwrite it!
                        return false;
                    } else {
                        // there is result read, but there is also result
                        // write before it so we are not overwriting
                        // anything
                        break;
                    }
                }
            } else {
                break;
            }
        }
    }

    if (!node.isDestinationOperation() || pSocket.isOutputPSocketResource()) {
        // If destination is ra register of gcu, or bypassed move
        // for which we are only interested at source at this call
        return true;
    }

    MoveNode* newNode = const_cast<MoveNode*>(&node);
    std::string opName = "";
    ProgramOperation* pOp = NULL;
    try {
        pOp = &newNode->destinationOperation();
    } catch (const InvalidData& e) {
        abortWithError(e.errorMessage());
    }
    int progOpCount = operandsWriten_.size();
    int firstCycle = 0;
    int lastCycle = 0;
    int triggering = -1;
    findRange(cycle, node, firstCycle, lastCycle, triggering);

    for (int i = firstCycle; i <= lastCycle; i++) {
        if (i >= progOpCount) {
            break;
        }
        if (operandsWriten_.at(i) != pOp &&
            operandsWriten_.at(i) != NULL) {
            return false;
        }
    }

    // triggering move may be different on a target machine then on
    // universal machine, test triggering using data from FUBrokers
    if (!(triggers /* || newNode->move().destination().isTriggering()*/)) {
        return true;
    }

    if (newNode->move().destination().isOpcodeSetting()) {
        opName = newNode->move().destination().operation().name();
    } else {
        // If target architecture has different opcode setting port
        // as universal machine, pick  a name of operation from a hint
        if (triggers) {
            opName = newNode->move().destination().hintOperation().name();
        }
        for (int i = cycle; i >= 0; i--) {
            // if there was opcode setting move earlier, pick operation from
            // the recorded ones
            if (MapTools::containsKey(opcodeSettingCycle_, i)) {
                opName = MapTools::valueForKey<std::string>(
                    opcodeSettingCycle_, i);
                break;
            }
        }
        if (opName == "") {
            std::string msg = "Using non opcodeSetting triggering move";
            msg += " without setting opcode first!";
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        }
    }

    if (!MapTools::containsKey(operationSupported_, opName)) {
        // Operation no supported by FU
        return false;
    }
    int pIndex = MapTools::valueForKey<int>(operationSupported_, opName);
    for (int i = 0; i < maximalLatency_; i++) {
        if ((cycle + i) >= size()) {
            return true;
        }
        for (int j = 0 ; j < numberOfResources_; j++) {
            if (operationPipelines_[pIndex][i][j] &&
                fuExecutionPipeline_[cycle + i][j]) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Always return true.
 *
 * @return true
 */
bool
ExecutionPipelineResource::isExecutionPipelineResource() const {
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of proper types.
 *
 * @return true Allways true, pipelines are internal to object.
 */
bool
ExecutionPipelineResource::validateDependentGroups() {
    return true;
}

/**
 * Tests if all resources in related resource groups are of proper types.
 *
 * @return true If all resources in related resource groups are
 *              Triggering PSockets - for now InputPSockets
 */
bool
ExecutionPipelineResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0, count = relatedResourceCount(i); j < count; j++) {
            if (!relatedResource(i, j).isInputPSocketResource())
                return false;
        }
    }
    return true;
}

/**
 * Return number of cycles current execution pipeline for FU contains.
 * Effectively, highest cycle in which any of the resources of an
 * FU is occupied plus 1.
 *
 * @return Number of cycles in pipeline.
 */
int
ExecutionPipelineResource::size() const {

#if 0
    // Breaks for load and store units with internal pipeline resources!!!
    if (cachedSize_ != INT_MIN ) {
        return cachedSize_;
    }
#endif
    int length = fuExecutionPipeline_.size() - 1;
    if (length == -1) {
        cachedSize_ = 0;
        return 0;
    }
    for (int i = length; i >= 0; i--) {
        for (int j = 0; j < numberOfResources_; j++) {
            if (fuExecutionPipeline_.at(i).at(j) == true) {
                cachedSize_ = i + 1;
                return i + 1;
            }
        }
    }
    cachedSize_ = 0;
    return 0;
}

/**
 * Sets usage of resource/port to true for given cycle and resource number
 * and particular pipeline.
 *
 * @param opName Name of operation to set resource for
 * @param cycle Cycle in which to set usage
 * @param index Index of resource/port in resource vector
 */
void
ExecutionPipelineResource::setResourceUse(
    const std::string& opName,
    const int cycle,
    const int resIndex) {

    if (cycle > maximalLatency_ || cycle < 0) {
        throw InvalidData(__FILE__, __LINE__, __func__, 
            "Trying to set resource use to cycle out of scope of "
            "FU pipeline!");
    }
    if (resIndex >= numberOfResources_ || resIndex < 0){
        throw InvalidData(__FILE__, __LINE__, __func__, 
            "Trying to set resource use for resource out of scope of "
            "FU pipeline!");
    }

    if (!MapTools::containsKey(operationSupported_, opName)) {
        ResourceTable newOp(maximalLatency_, numberOfResources_);
        operationPipelines_.push_back(newOp);
        operationSupported_[opName] = operationPipelines_.size() - 1;
    }
    int pIndex = MapTools::valueForKey<int>(operationSupported_, opName);
    operationPipelines_[pIndex][cycle][resIndex] = true;
}

/**
 * For given MoveNode and cycle find cycle in which other MoveNodes
 * of same program operation are already scheduled. The closest ones.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @param first Will return address of closest previous cycle already
 * used by MoveNode of same program operation
 * @param last Will return address of closest later cycle already
 * used by MoveNode of same program operation
 */
void
ExecutionPipelineResource::findRange(
    const int cycle,
    const MoveNode& node,
    int& first,
    int& last,
    int& triggering) const {

    ProgramOperation* pOp = NULL;
    try {
        pOp = &node.destinationOperation();
    } catch (const InvalidData& e) {
        abortWithError(e.errorMessage());
    }

    int maxCycle = highestKnownCycle();
    int progOpCount = operandsWriten_.size();
    if (maxCycle < progOpCount) {
        maxCycle = progOpCount;
    }
    if (maxCycle < cycle) {
        maxCycle = cycle + 1;
    }
    last = INT_MAX;
    first = -1;
    triggering = -1;
    for (int k = 0; k < pOp->inputMoveCount(); k++) {
        // find closest cycle from PO to given node, before and after
        if (pOp->inputMove(k).isScheduled()) {
            if (pOp->inputMove(k).move().isTriggering()) {
                triggering = pOp->inputMove(k).cycle();
            }
            if (pOp->inputMove(k).cycle() > cycle &&
                pOp->inputMove(k).cycle() < last) {
                last = pOp->inputMove(k).cycle();
            }
            if (pOp->inputMove(k).cycle() < cycle &&
                pOp->inputMove(k).cycle() > first) {
                first = pOp->inputMove(k).cycle();
            }
        }
    }
    if (first == -1) {
        first = cycle;
    }
    if (last == INT_MAX) {
        last = cycle;
    }
}

/**
 * Returns the highest cycle known to Execution Pipeline to be used by either
 * pipeline resources or some operands, trigger or result read/write
 *
 * @return Highest cycle in which the pipeline is known to be used.
 */
int
ExecutionPipelineResource::highestKnownCycle() const {

    int maximum = operandsWriten_.size() - 1;
    while (maximum >= 0 && operandsWriten_.at(maximum) == NULL) {
        maximum--;
    }
    int maxResults = resultWriten_.size() - 1;
    while (maxResults >= 0 && resultWriten_.at(maxResults).first == NULL) {
        maxResults--;
    }
    int maxResultReads = resultRead_.size() -1;
    while (maxResultReads >= 0 &&
        resultRead_.at(maxResultReads).first == NULL) {
        maxResultReads--;
    }
    int tmp = std::max(maximum, maxResults);
    tmp = std::max(tmp, maxResultReads);
    // size returns count of cycle, max cycle address needs -1
    return std::max(tmp, size() - 1);
}

/**
 * Returns a cycle in which result of next program operation will be
 * writen to result.
 *
 * @param cycle Cycle from which to start testing.
 * @return Cycle in which next result will be writen, overwriting curent one.
 */
int
ExecutionPipelineResource::nextResultCycle(int cycle, const MoveNode& node)
    const {

    if (!node.isSourceOperation()) {
        throw InvalidData(__FILE__, __LINE__, __func__, 
            "Trying to get next result for move that is not "
            "in ProgramOperation");
    }
    ProgramOperation* sourcePo = NULL;
    if (node.isSourceOperation()) {
        sourcePo = &node.sourceOperation();
    }
    
    int resSize = resultWriten_.size();

    if (cycle > resSize) {
        return INT_MAX;
    }
    for (int i = cycle; i < resSize; i++) {
        if (resultWriten_.at(i).second > 0 ) {
            if (resultWriten_.at(i).first != sourcePo) {
                return i;
            } 
        }
    } 
    return INT_MAX;
}
