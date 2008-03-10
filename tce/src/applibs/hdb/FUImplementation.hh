/**
 * @file FUImplementation.hh
 *
 * Declaration of FUImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_IMPLEMENTATION_HH
#define TTA_FU_IMPLEMENTATION_HH

#include <string>
#include <vector>
#include <map>

#include "HWBlockImplementation.hh"
#include "Exception.hh"

namespace HDB {

class FUEntry;
class FUExternalPort;
class FUPortImplementation;

/**
 * Represents the implementation specific properties of an FU in HDB.
 */
class FUImplementation : public HWBlockImplementation {
public:
    /// Parameter of FU implementation.
    struct Parameter {
        std::string name; ///< Name of the parameter.
        std::string type; ///< Type of the parameter.
        std::string value; ///< Value of the parameter.
    };

    FUImplementation(
        const std::string& name,
        const std::string& opcodePort,
        const std::string& clkPort,
        const std::string& rstPort,
        const std::string& glockPort, 
        const std::string& glockReqPort);
    virtual ~FUImplementation();

    FUImplementation(const FUImplementation& o);

    void setOpcodePort(const std::string& name);
    std::string opcodePort() const;
    void setGlockReqPort(const std::string& name);
    std::string glockReqPort() const;

    void setOpcode(const std::string& operation, int opcode);
    void unsetOpcode(const std::string& operation);
    int opcodeCount() const;
    std::string opcodeOperation(int index) const
        throw (OutOfRange);
    bool hasOpcode(const std::string& operation) const;
    int opcode(const std::string& operation) const
        throw (KeyNotFound);
    int maxOpcodeWidth() const;

    void addArchitecturePort(FUPortImplementation* port);
    void deleteArchitecturePort(FUPortImplementation* port)
        throw (InstanceNotFound);
    void addExternalPort(FUExternalPort* port);
    void deleteExternalPort(FUExternalPort* port)
        throw (InstanceNotFound);

    int architecturePortCount() const;
    int externalPortCount() const;
    
    FUPortImplementation& architecturePort(int index) const
        throw (OutOfRange);
    FUPortImplementation& portImplementationByArchitectureName(
        const std::string& architectureName) const
        throw (InstanceNotFound);
    FUExternalPort& externalPort(int index) const
        throw (OutOfRange);

    void addParameter(
        const std::string& name,
        const std::string& type,
        const std::string& value)
        throw (IllegalParameters);
    void removeParameter(const std::string& name);
    int parameterCount() const;
    Parameter parameter(int index) const
        throw (OutOfRange);
    bool hasParameter(const std::string& name) const;

private:
    /// Vector type for FUPortImplementation.
    typedef std::vector<FUPortImplementation*> PortTable;
    /// Vector type for FUExternalPort.
    typedef std::vector<FUExternalPort*> ExternalPortTable;
    /// Map type for operation codes.
    typedef std::map<std::string, int> OpcodeMap;
    /// Vector type for parameter names.
    typedef std::vector<Parameter> ParameterTable;

    /// Name of the opcode port.
    std::string opcodePort_;
    /// Name of the global lock request port.
    std::string glockReqPort_;

    /// Contains the architectural ports.
    PortTable ports_;
    /// Contains the external ports.
    ExternalPortTable externalPorts_;
    /// Contains the operation codes.
    OpcodeMap opcodes_;
    /// Contains the parameters.
    ParameterTable parameters_;
};
}

#endif
