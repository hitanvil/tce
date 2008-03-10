/**
 * @file ControlFlowEdge.cc
 *
 * Prototype of graph-based program representation: implementation of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ControlFlowEdge.hh"

/**
 * Constructor creates Control Flow Edge of given type
 *
 * @param edgeID The numerical ID the edge will have
 * @param edgePredicate The truth value of edge (or normal)
 * @param isJump Define if edge represents jump, if false the it is call
 */
ControlFlowEdge::ControlFlowEdge(
    const int edgeID,
    CFGEdgePredicate edgePredicate,
    bool isJump) :
    GraphEdge(edgeID), edgePredicate_(edgePredicate), isJumpEdge_(isJump) {
}

/**
 * Destructor
 */
ControlFlowEdge::~ControlFlowEdge() { }

/**
 * Returns type as a string. Helper for graph output to dot file.
 *
 * @return String representing type of edge.
 */
std::string
ControlFlowEdge::toString() const {
    std::string result = "";
    if (isJumpEdge()) {
        result += "Jump_";
    }
    if (isCallPassEdge()) {
        result += "CallPass_";
    }
    if (isNormalEdge()) {
        result += "normal";
    }
    if (isTrueEdge()) {
        result += "true";
    }
    if (isFalseEdge()) {
        result += "false";
    }
    if (isLoopBreakEdge()) {
        result += "break";
    }
    return result;
}

/**
 * Returns true if object represents ControlFlowEdge.
 *
 * @return True if object is control flow edge
 */
bool
ControlFlowEdge::isControlFlowEdge() const {
    return true;
}
/**
 * Returns true if the edge is representing unconditional jump
 *
 * @return True if jump is not guarded
 */
bool
ControlFlowEdge::isNormalEdge() const {
    return edgePredicate_ == CFLOW_EDGE_NORMAL;
}

/**
 * Returns true if edge represents conditional control flow that is
 * taken if condition evaluates to true.
 *
 * @return True if edge represents True path
 */
bool
ControlFlowEdge::isTrueEdge() const {
    return edgePredicate_ == CFLOW_EDGE_TRUE;
}
/**
 * Returns true if edge represents conditional control flow that is
 * taken if condition evaluates to false.
 *
 * @return True if edge represents False path
 */
bool
ControlFlowEdge::isFalseEdge() const {
    return edgePredicate_ == CFLOW_EDGE_FALSE;
}
/**
 * Returns true if edge represents control flow over the call in code.
 *
 * @return True if edge is split in basic block when call happened.
 */
bool
ControlFlowEdge::isCallPassEdge() const {
    return !isJumpEdge();
}

/**
 * Returns true if the edge is representing jump.
 *
 * @return True if the edge represents jump.
 */
bool
ControlFlowEdge::isJumpEdge() const {
    return isJumpEdge_ == true;
}

/**
 * Returns true if edge was added to break infinite
 * loop for control dependence computation.
 *
 * @return True if edge was added to break infinite loop
 */
bool
ControlFlowEdge::isLoopBreakEdge() const {
    return edgePredicate_ == CFLOW_EDGE_LOOP_BREAK;
}

