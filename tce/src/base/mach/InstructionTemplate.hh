/**
 * @file InstructionTemplate.hh
 *
 * Declaration of class InstructionTemplate.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_TEMPLATE_HH
#define TTA_INSTRUCTION_TEMPLATE_HH

#include <string>
#include <vector>

#include "MachinePart.hh"
#include "Exception.hh"

namespace TTAMachine {

class Bus;
class Machine;
class ImmediateUnit;
class TemplateSlot;

class InstructionTemplate : public Component {
public:
    InstructionTemplate(const std::string& name, Machine& owner)
        throw (ComponentAlreadyExists, InvalidName);
    InstructionTemplate(const ObjectState* state, Machine& owner)
        throw (ObjectStateLoadingException);
    virtual ~InstructionTemplate();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void addSlot(
        const std::string& slotName,
        int width,
        ImmediateUnit& dstUnit)
        throw (InstanceNotFound, IllegalRegistration, ComponentAlreadyExists,
               OutOfRange);
    virtual void removeSlot(const std::string& slotName);
    virtual void removeSlots(const ImmediateUnit& dstUnit);

    virtual int slotCount() const;
    virtual TemplateSlot* slot(int index) const;

    virtual bool usesSlot(const std::string& slotName) const;
    virtual bool destinationUsesSlot(
        const std::string& slotName,
        const ImmediateUnit& dstUnit) const;
    virtual int numberOfDestinations() const;
    virtual bool isOneOfDestinations(const ImmediateUnit& dstUnit) const;
    virtual ImmediateUnit* destinationOfSlot(
        const std::string& slotName) const
        throw (InstanceNotFound);

    virtual int numberOfSlots(const ImmediateUnit& dstUnit) const;
    virtual std::string slotOfDestination(
        const ImmediateUnit& dstUnit,
        int index) const
        throw (OutOfRange);

    virtual int supportedWidth() const;
    virtual int supportedWidth(const ImmediateUnit& dstUnit) const;
    virtual int supportedWidth(const std::string& slotName) const;

    virtual bool isEmpty() const;

    virtual void setMachine(Machine& machine)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for instruction template.
    static const std::string OSNAME_INSTRUCTION_TEMPLATE;

private:
    /// Container for TemplateSlots.
    typedef std::vector<TemplateSlot*> SlotTable;

    void deleteAllSlots();
    TemplateSlot* templateSlot(const std::string& slotName) const;

    /// Contains all the slots of the instruction template.
    SlotTable slots_;
};
}

#endif
