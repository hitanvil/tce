/**
 * @file TTASim.cc
 *
 * Implementation of ttasim.
 *
 * The command line version of the TTA Simulator.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "SimulatorInterpreter.hh"
#include "SimulatorInterpreterContext.hh"
#include "SimulatorCmdLineOptions.hh"
#include "SimulatorConstants.hh"
#include "SimulatorToolbox.hh"
#include "LineReader.hh"
#include "LineReaderFactory.hh"
#include "SimulatorConstants.hh"
#include "StringTools.hh"
#include "Application.hh"
#include "Exception.hh"
#include "Listener.hh"
#include "Informer.hh"
#include "SimulationEventHandler.hh"
#include "FileSystem.hh"
#include "CompiledSimInterpreter.hh"

/**
 * A handler class for Ctrl-c signal.
 *
 * Stops the simulation (if it's running).
 */
class SimulationStopper : public Application::UnixSignalHandler {
public:
    /**
     * Constructor.
     *
     * @param target The target SimulatorFrontend instance.
     */
    SimulationStopper(SimulatorFrontend& target) : target_(target) {
    }

    /**
     * Stops the simulation.
     */
    virtual void execute(int) {
        target_.prepareToStop(SRE_USER_REQUESTED);
    }
private:
    /// Simulator frontend to use when stopping the simulation.
    SimulatorFrontend& target_;
};

/**
 * Class that catches simulated program runtime error events and prints
 * the error reports to stderr.
 */
class RuntimeErrorReporter : public Listener {
public:
    /**
     * Constructor.
     *
     * Registers itself to the SimulationEventHandler to listen to 
     * runtime error events.
     *
     * @param target The target SimulatorFrontend instance.
     */
    RuntimeErrorReporter(SimulatorFrontend& target) : 
        Listener(), target_(target) {
        target.eventHandler().registerListener(
            SimulationEventHandler::SE_RUNTIME_ERROR, this);
    }
    
    virtual ~RuntimeErrorReporter() {
        target_.eventHandler().unregisterListener(
            SimulationEventHandler::SE_RUNTIME_ERROR, this);
    }

    /**
     * Handles the runtime error event by printing and error report to
     * stderr and stopping simulation in case it's a fatal error.
     *
     * @todo TextGenerator.
     */
    virtual void handleEvent() {
        size_t minorErrors = SimulatorToolbox::programErrorReportCount(
            SimulatorToolbox::RES_MINOR);
        size_t fatalErrors = SimulatorToolbox::programErrorReportCount(
            SimulatorToolbox::RES_FATAL);
        InstructionAddress currentPC = target_.programCounter();
        InstructionAddress lastPC = target_.lastExecutedInstruction();

        if (minorErrors > 0) {
            for (size_t i = 0; i < minorErrors; ++i) {
                std::cerr << "warning: runtime error: "
                          << SimulatorToolbox::programErrorReport(
                              SimulatorToolbox::RES_MINOR, i)
                          << std::endl;
            }
        }

        if (fatalErrors > 0) {
            for (size_t i = 0; i < fatalErrors; ++i) {
                std::cerr << "error: runtime error: "
                          << SimulatorToolbox::programErrorReport(
                              SimulatorToolbox::RES_FATAL, i) 
                          << std::endl;
            }
            target_.prepareToStop(SRE_RUNTIME_ERROR);
        }
        std::cerr 
            << "Current PC: " << currentPC << " last PC: " << lastPC
            << std::endl;
        SimulatorToolbox::clearProgramErrorReports();
    }

private:
    /// Simulator frontend to use.
    SimulatorFrontend& target_;
};

/**
 * Executes the given script string in the script interpreter and
 * prints out possible results.
 *
 * @param interpreter Interpreter to use.
 * @param scriptString Script string to execute.
 */
void interpreteAndPrintResults(
    SimulatorInterpreter& interpreter, const std::string& scriptString) {
    interpreter.interpret(scriptString);
    if (interpreter.result().size() > 0)
        std::cout << interpreter.result() << std::endl;
}

/**
 * Main function.
 *
 * Parses the command line and figures out whether to start the interactive
 * debugging mode or not.
 *
 * @param argc The command line argument count.
 * @param argv The command line arguments (passed to the interpreter).
 * @return The return status.
 */
int main(int argc, char* argv[]) {

    Application::initialize();    
    
    boost::shared_ptr<SimulatorFrontend> simFront;
    boost::shared_ptr<SimulatorInterpreter> interpreter;
    /// @todo: read command line options and initialize the 
    /// simulator (frontend) using the given values.
    SimulatorCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) {
        std::cerr << i.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    
    simFront.reset(new SimulatorFrontend(options.fastSimulationEngine()));
    
    SimulatorInterpreterContext context(*simFront);
    LineReader* reader = LineReaderFactory::lineReader();
    assert(reader != NULL);
    reader->initialize(SIM_COMMAND_PROMPT);
    reader->setInputHistoryLog(SIM_DEFAULT_COMMAND_LOG);

    if (!options.fastSimulationEngine()) {
        interpreter.reset(new SimulatorInterpreter(
            argc, argv, context, *reader));
    } else {
        interpreter.reset(new CompiledSimInterpreter(
            argc, argv, context, *reader));
    }

    // check if there is an initialization file in user's home dir and 
    // execute it
    const std::string personalInitScript =
        FileSystem::homeDirectory() + DIR_SEPARATOR + SIM_INIT_FILE_NAME;
    if (FileSystem::fileExists(personalInitScript)) {
        interpreter->processScriptFile(personalInitScript);
    }   
    
    std::string machineToLoad = options.machineFile();
    std::string programToLoad = options.programFile();
    const std::string scriptString = options.scriptString();
    
    if (options.numberOfArguments() != 0) {
        std::cerr << SimulatorToolbox::textGenerator().text(
            Texts::TXT_ILLEGAL_ARGUMENTS).str() << std::endl;
        return EXIT_FAILURE;
    }  

    // check if there is an initialization file in the current dir and
    // execute it
    const std::string currentDirInitScript =
        FileSystem::currentWorkingDir() + DIR_SEPARATOR + SIM_INIT_FILE_NAME;
    if (FileSystem::fileExists(currentDirInitScript)) {
        interpreter->processScriptFile(currentDirInitScript);
    }

    bool interactiveMode = options.debugMode();

    /// Catch runtime errors and print them out to the simulator console.
    RuntimeErrorReporter errorReporter(*simFront);

    SimulationStopper ctrlcHandler(*simFront);
    Application::setCtrlcHandler(ctrlcHandler);

    if (machineToLoad != "") {
        interpreteAndPrintResults(
            *interpreter, std::string("mach " ) + machineToLoad);
    }

    if (programToLoad != "") {
        interpreteAndPrintResults(
            *interpreter, std::string("prog " ) + programToLoad);
        if (scriptString == "") {
            // by default, if program is given, start simulation immediately
            interpreteAndPrintResults(*interpreter, "run");
        }
    }

    if (scriptString != "") {
        interpreteAndPrintResults(*interpreter, scriptString);
    }
    
    std::string command = "";
    while (interactiveMode && !interpreter->isQuitCommandGiven()) {
        try {
            command = reader->readLine();
        } catch (const EndOfFile&) {
            // execute the actual interpreter quit command in case user
            // pressed ctrl-d or the input file ended, to make sure
            // all uninitialization routines are executed correctly
            interpreter->interpret(SIM_INTERP_QUIT_COMMAND);
            std::cout << interpreter->result() << std::endl;
            break;
        }
        command = StringTools::trim(command);
        if (command == "") {
            continue;
        }
        interpreter->interpret(command);
        if (interpreter->result().size() > 0)
            std::cout << interpreter->result() << std::endl;	
    }

    delete reader;
    reader = NULL;
    Application::restoreCtrlcHandler();
    return EXIT_SUCCESS;
}
