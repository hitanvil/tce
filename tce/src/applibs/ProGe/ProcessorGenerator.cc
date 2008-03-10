/**
 * @file ProcessorGenerator.cc
 *
 * Implementation of ProcessorGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @author Esa Määttä 2007 (esa.maatta@tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <set>
#include <list>
#include <string>
#include <fstream>
#include <algorithm>
#include <boost/format.hpp>

#include "ProcessorGenerator.hh"
#include "NetlistGenerator.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "VHDLNetlistWriter.hh"
#include "BlockSourceCopier.hh"

#include "ControlUnit.hh"
#include "AddressSpace.hh"
#include "SpecialRegisterPort.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "BinaryEncoding.hh"
#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"
#include "MachineImplementation.hh"
#include "RFEntry.hh"
#include "RFArchitecture.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"

#include "FileSystem.hh"
#include "MathTools.hh"
#include "Environment.hh"

using std::string;
using boost::format;

const string CALL = "CALL";
const string JUMP = "JUMP";

using std::set;
using std::endl;
using namespace TTAMachine;
using namespace IDF;
using namespace HDB;

namespace ProGe {

/**
 * The constructor.
 */
ProcessorGenerator::ProcessorGenerator() {
}


/**
 * The destructor.
 */
ProcessorGenerator::~ProcessorGenerator() {
}


/**
 * Generates the processor.
 *
 * @param language The HDL to be used.
 * @param machine The target architecture to generate an implementation for.
 * @param implementation The implementation definition to use.
 * @param plugin The IC/decoder generator plugin.
 * @param imemWidthInMAUs Width of the instruction memory in MAUs.
 * @param dstDirectory The destination directory.
 * @param outputStream Stream where warnings etc. information output is
 *                     written.
 * @exception IOException If an IO error occurs.
 * @exception InvalidData If implementation of a block defined in IDF is
 *                        not available or if HDB is erroneous or if the
 *                        GCU/IC plugin is incompatible with the machine.
 * @exception IllegalMachine If the machine is illegal.
 * @exception OutOfRange If the given instruction memory width is not positive.
 * @exception InstanceNotFound HDB is missing some data.
 */
void
ProcessorGenerator::generateProcessor(
    HDL language,
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& implementation,
    ICDecoderGeneratorPlugin& plugin,
    int imemWidthInMAUs,
    const std::string& dstDirectory,
    std::ostream& outputStream)
    throw (IOException, InvalidData, IllegalMachine, OutOfRange,
           InstanceNotFound) {

    // validate the machine
    validateMachine(machine, outputStream);
    // check the compatibility of the plugin
    plugin.verifyCompatibility();
    // check that IU implementation latencies are compatible with the
    // IC/GCU plugin
    checkIULatencies(machine, implementation, plugin);

    NetlistGenerator netlistGenerator(machine, implementation, plugin);
    Netlist* netlist = netlistGenerator.generate(imemWidthInMAUs);

    string pluginDstDir = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
        "gcu_ic";
    bool created = FileSystem::createDirectory(pluginDstDir);
    if (!created) {
        delete netlist;
        string errorMsg = "Unable to create directory " + pluginDstDir;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    plugin.generate(pluginDstDir, netlistGenerator);

    NetlistWriter* writer;
    if (language == VHDL) {
        writer = new VHDLNetlistWriter(*netlist);
    } else {
        assert(false);
    }

    string topLevelDir = "";
    if (language == VHDL) {
        topLevelDir = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
            "vhdl";
    } else {
        assert(false);
    }

    writer->write(topLevelDir);

    delete writer;
    writer = NULL;
    delete netlist;
    netlist = NULL;

    BlockSourceCopier copier(implementation);
    copier.copy(dstDirectory);

    if (language == VHDL) {
        generateGlobalsPackage(
            machine, plugin.bem(), imemWidthInMAUs, topLevelDir);
    } else {
        assert(false);
    }
}

/**
 * Generates the package that defines global constants used in processor
 * definition files.
 *
 * @param machine The machine.
 * @param bem The binary encoding map.
 * @param imemWidthInMAUs Width of the instruction memory in MAUs.
 * @param dstDirectory The destination directory.
 * @exception IOException If an IO error occurs.
 */
void
ProcessorGenerator::generateGlobalsPackage(
    const TTAMachine::Machine& machine,
    const BinaryEncoding& bem,
    int imemWidthInMAUs,
    const std::string& dstDirectory)
    throw (IOException) {

    string dstFile = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
        "globals_pkg.vhdl";
    bool created = FileSystem::createFile(dstFile);
    if (!created) {
        string errorMsg = "Unable to create file " + dstFile;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    int iMemMAUWidth = machine.controlUnit()->addressSpace()->width();

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    stream << "package globals is" << endl;
    stream << "  -- instruction width" << endl;
    stream << "  constant INSTRUCTIONWIDTH : positive := " << bem.width()
           << ";" << endl;
    stream << "  -- address width of the instruction memory" << endl;
    stream << "  constant IMEMADDRWIDTH : positive := "
           << iMemAddressWidth(machine) << ";" << endl;
    stream << "  -- width of the MAU of instruction memory" << endl;
    stream << "  constant IMEMMAUWIDTH : positive := " << iMemMAUWidth << ";"
           << endl;
    stream << "  -- width of the instruction memory in MAUs" << endl;
    stream << "  constant IMEMWIDTHINMAUS : positive := " << imemWidthInMAUs
           << ";" << endl;
    stream << "  -- clock period" << endl;
    stream << "  constant PERIOD : time := 10 ns;" << endl;
    stream << "end globals;" << endl;

    stream.close();
}


/**
 * Validates the machine for compatibility with the given block
 * implementations.
 *
 * If the target architecture contains errors (incomplete definition) or if
 * its structure is not compatible with this HDL generator, this method
 * throws IllegalMachine exception. For less serious errors (such as
 * non-critical bit width discrepancies), warning messages are written to
 * the given stream.
 *
 * @param machine The machine to validate.
 * @param outputStream The stream.
 * @exception IllegalMachine If there is a fundamental error in the machine.
 */
void
ProcessorGenerator::validateMachine(
    const TTAMachine::Machine& machine,
    std::ostream& outputStream)
    throw (IllegalMachine) {

    MachineValidator validator(machine);
    set<MachineValidator::ErrorCode> errorsToCheck;
    errorsToCheck.insert(MachineValidator::GCU_MISSING);
    errorsToCheck.insert(MachineValidator::GCU_AS_MISSING);
    errorsToCheck.insert(MachineValidator::USED_IO_NOT_BOUND);
    errorsToCheck.insert(MachineValidator::DIFFERENT_PORT_FOR_JUMP_AND_CALL);
    errorsToCheck.insert(MachineValidator::PC_PORT_MISSING);
    errorsToCheck.insert(MachineValidator::RA_PORT_MISSING);
    errorsToCheck.insert(MachineValidator::PC_AND_RA_PORTS_HAVE_UNEQUAL_WIDTH);
    errorsToCheck.insert(
        MachineValidator::IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC);
    
    MachineValidatorResults* results = validator.validate(errorsToCheck);

    for (int i = 0; i < results->errorCount(); i++) {
        MachineValidator::ErrorCode code = results->error(i).first;
        string errorMsg = results->error(i).second;
        if (code ==
            MachineValidator::IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC) {
            outputStream << "Warning: " << errorMsg
                         << " ProGe uses the greater value." << endl;
        } else {
            string msg = "Error: " + errorMsg;
            delete results;
            throw IllegalMachine(__FILE__, __LINE__, __func__, msg);
        }
    }
    delete results;
}


/**
 * Checks that the latencies of the HW implementations of the immediate
 * units are compatible with the given IC/GCU generator plugin.
 *
 * @param machine The machine.
 * @param implementation The machine implementation.
 * @param plugin The plugin.
 * @exception InvalidData If the latencies are incompatible.
 */
void
ProcessorGenerator::checkIULatencies(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& implementation,
    const ICDecoderGeneratorPlugin& plugin)
    throw (Exception) {

    Machine::ImmediateUnitNavigator iuNav = machine.immediateUnitNavigator();
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit* iu = iuNav.item(i);
        RFImplementationLocation& impl = implementation.iuImplementation(
            iu->name());
        
        RFEntry* entry = NULL;
        try {
            HDBManager& manager = HDBRegistry::instance().hdb(impl.hdbFile());
            entry = manager.rfByEntryID(impl.id());
        } catch (const KeyNotFound& e) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, e.errorMessage());
        }
        
        if (!entry->hasArchitecture()) {
            delete entry;
            format text(
                "RF entry %1% does not have architecture defined in HDB "
                "%2%.");
            text % impl.id() % impl.hdbFile();
            throw InvalidData(__FILE__, __LINE__, __func__, text.str());
        }

        RFArchitecture& architecture = entry->architecture();
        int implLatency = architecture.latency();
        int requiredLatency = plugin.requiredRFLatency(*iu);
        delete entry;

        if (implLatency != requiredLatency) {
            format errorMsg(
                "Latency of the implementation of immediate unit %1% is %2%,"
                " required latency is %3%.");
            errorMsg % iu->name() % implLatency % requiredLatency;
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg.str());
        }
    }
}                
                

/**
 * Returns the width of the memory address of instruction memory of the
 * given machine.
 *
 * @param mach The machine.
 * @return The bit width.
 * @exception InvalidData If the GCU of the machine does not have an address
 *                        space.
 */
int
ProcessorGenerator::iMemAddressWidth(const TTAMachine::Machine& mach) {

    ControlUnit* gcu = mach.controlUnit();
    // must have been already caught in main method: generateProcessor
    assert(gcu != NULL);

    AddressSpace* iMem = gcu->addressSpace();
    // must have been already caught in main method: generateProcessor
    assert(iMem != NULL);

    if (gcu->hasReturnAddressPort()) {
        return std::max(
            static_cast<int>(MathTools::requiredBits(iMem->end())),
            gcu->returnAddressPort()->width());
    } else {
        if (gcu->hasOperation(CALL)) {
            FUPort* pcPort = gcu->operation(CALL)->port(1);
            if (pcPort != NULL) {
                return std::max(
                    static_cast<int>(MathTools::requiredBits(iMem->end())),
                    pcPort->width());
            }
        } else if (gcu->hasOperation(JUMP)) {
            FUPort* pcPort = gcu->operation(JUMP)->port(1);
            if (pcPort != NULL) {
                return std::max(
                    static_cast<int>(MathTools::requiredBits(iMem->end())),
                    pcPort->width());
            }
        }
    }

    return MathTools::requiredBits(iMem->end());
}


/**
 * Returns the width of the instruction memory of the given machine.
 *
 * @param mach The machine.
 * @return The bit width.
 */
int
ProcessorGenerator::iMemWidth(
    const TTAMachine::Machine& mach, int imemWidthInMAUs) {

    ControlUnit* gcu = mach.controlUnit();
    // must have been already caught in main method: generateProcessor
    assert(gcu != NULL);

    AddressSpace* iMem = gcu->addressSpace();
    // must have been already caught in main method: generateProcessor
    assert(iMem != NULL);

    return iMem->width() * imemWidthInMAUs;
}

} // namespace ProGe
