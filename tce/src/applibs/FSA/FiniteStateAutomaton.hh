/**
 * @file FiniteStateAutomaton.hh
 *
 * Declaration of FiniteStateAutomaton class.
 *
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FINITE_STATE_AUTOMATON_HH
#define TTA_FINITE_STATE_AUTOMATON_HH

#include <string>
#include <set>
#include <map>
#include <vector>

#include "Exception.hh"

/**
 * Generic finite state automaton (FSA).
 *
 * This is a generic structure for FSA. The building and initialization of
 * the states is the responsibility of derived classes.
 */
class FiniteStateAutomaton {
public:
    /// Friend due to highly optimized compiled simulation versions of
    /// the conflict detection functions.
    friend class FSAFUResourceConflictDetector;
    /// Type used for indexing the transitions.
    typedef int FSAStateTransitionIndex;
    /// Type used for indexing the states.
    typedef int FSAStateIndex;
    /// Type for a set of state indices.
    typedef std::set<FSAStateIndex> FSAStateIndexSet;
    /// A state id which denotes an illegal state.
    static const FSAStateIndex ILLEGAL_STATE = -1;    
    /// A state id which denotes an unknown (unresolved) state. Used for
    /// lazy construction of states.
    static const FSAStateIndex UNKNOWN_STATE = -2;

    FiniteStateAutomaton(
        FSAStateTransitionIndex defaultState = ILLEGAL_STATE,
        int transitionCount = 0);
    virtual ~FiniteStateAutomaton();

    virtual std::string transitionName(FSAStateTransitionIndex transition)
        const;

    virtual std::string stateName(FSAStateIndex state) const;

    virtual FSAStateTransitionIndex transitionIndex(
        const std::string& transitionName) const 
        throw (KeyNotFound);

    virtual FSAStateIndex destinationState(
        FSAStateIndex source, 
        FSAStateTransitionIndex transition);

    virtual bool isLegalTransition(
        FSAStateIndex source,
        FSAStateTransitionIndex transition);

    virtual FSAStateIndex addState();

    virtual FSAStateTransitionIndex addTransitionType(
        const std::string& name);

    virtual void setTransitionName(
        FSAStateTransitionIndex transition, const std::string& name);

    virtual void setTransition(
        FSAStateIndex source, FSAStateIndex destination, 
        FSAStateTransitionIndex transition);

    virtual std::string toDotString() const;

    virtual FSAStateIndex startState() const;


protected:

    /// Vector which stores the target states of for each transition type.
    typedef std::vector<FSAStateIndex> TransitionVector;

    /// Type for storing the transitions of each state. The vector
    /// stores for each state (indexed from 0) a vector with elements as
    /// many there are transitions. The value of the element is the target
    /// state to which the transition leads to, or ILLEGAL_STATE_INDEX if
    /// there is no transition for that transition type from that state.
    /// Using this structure it's possible to find the target state using two
    /// table (vector) lookups.
    typedef std::vector<TransitionVector> TransitionMap;

    /// The state transitions. In protected to allow fast access from
    /// derived classes.
    TransitionMap transitions_;

private:

    /// Type for finding the index for a transition using its name.
    typedef std::map<std::string, FSAStateTransitionIndex> 
    TransitionNameIndex;
    /// Count of states in the automaton.
    int stateCount_;
    /// Count of different transition types in the automaton.
    int transitionTypeCount_;
    /// Names of the state transitions types (indexed from 0).
    std::vector<std::string> transitionNames_;
    /// Indices of the state transition types.
    TransitionNameIndex transitionIndices_;
    /// The default target state for state transitions.
    const FSAStateTransitionIndex defaultState_;
};

#endif
