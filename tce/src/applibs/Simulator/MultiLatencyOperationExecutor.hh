/**
 * @file MultiLatencyOperationExecutor.hh
 *
 * Declaration of MultiLatencyOperationExecutor class.
 *
 * @author Pekka Jääskeläinen 2008 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MULTILATENCY_OPERATION_EXECUTOR_HH
#define TTA_MULTILATENCY_OPERATION_EXECUTOR_HH

#include <string>
#include <list>

#include "OperationExecutor.hh"
#include "SequenceTools.hh"
#include "SimValue.hh"
#include "HWOperation.hh"

class OperationContext;

/**
 * OperationExecutor that supports multi-output operation pipelines with 
 * different latencies for results.
 */
class MultiLatencyOperationExecutor : public OperationExecutor {
public:
    MultiLatencyOperationExecutor(
        FUState& parent, TTAMachine::HWOperation& hwOp);
    virtual ~MultiLatencyOperationExecutor();

    virtual void startOperation(Operation& op);

    virtual void advanceClock();
    virtual OperationExecutor* copy();
    virtual void setContext(OperationContext& context);

private:
    /// Assignment not allowed.
    MultiLatencyOperationExecutor& operator=(
        const MultiLatencyOperationExecutor&);

    /**
     * Models the latency of an operation result.
     */
    class PendingResult {
    public:
        /**
         * Constructor.
         *
         * @param result A result of an operation.
         * @param target The target where to write the result.
         * @param cyclesToGo How many cycles to wait before writing.
         */
        PendingResult(PortState& target, int cyclesToGo) : 
            target_(target), cyclesToGo_(cyclesToGo) {
        }

        /**
         * Signals a cycle advance.
         *
         * In case the set latency has passed, writes the result to the
         * target. This method should not be called anymore after the cycle 
         * count has reached 0!
         *
         * @return Return true in case the latency was passed and the
         * result is not pending anymore, thus the object can be deleted.
         */
        bool
        advanceCycle() {
            --cyclesToGo_;
            if (cyclesToGo_ <= 0) {
                target_.setValue(result_);
                return true;
            } else {
                return false;
            }            
        }

        SimValue& 
        resultValue() {
            return result_;
        }

    private:
        /// The value that will be written to the target after the
        /// latency has passed.
        SimValue result_;
        /// The target to which the result will be written after the
        /// latency.
        PortState& target_;
        /// How many cycles to wait until the result will be written to
        /// the target.
        int cyclesToGo_;        
    };

    /// Queue for pending (waiting for the latency to pass) results.
    typedef std::list<PendingResult*> PendingResultQueue;
    /// Buffer for results that have latency cycles left. After cycles
    /// have passed, the results are removed from the queue.
    PendingResultQueue pendingResults_;
    /// Operation context.
    OperationContext* context_;
    /// The hardware operation this executor simulates.
    TTAMachine::HWOperation& hwOperation_;
    /// Vector for the inputs and ouputs for the operation, required
    /// by the OSAL interface. Allocated here to avoid runtime overhead
    /// of allocation.
    SimValue* iovec_[EXECUTOR_MAX_OPERAND_COUNT];

};

#endif
