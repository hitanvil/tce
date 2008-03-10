/**
 * @file SimulatorToolbox.cc
 *
 * Definition of SimulatorToolbox class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J��skel�inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <cstddef>

#include "boost/regex.hpp"

#include "Application.hh"
#include "SimulatorTextGenerator.hh"
#include "SimulatorToolbox.hh"
#include "OperationPool.hh"
#include "SimulationEventHandler.hh"

OperationPool* SimulatorToolbox::pool_(NULL);
SimulatorTextGenerator SimulatorToolbox::textGenerator_;
SimulationEventHandler* SimulatorToolbox::eventHandler_(NULL);
SimulatorToolbox::ProgramErrorDescriptionList 
SimulatorToolbox::programErrorReports_;

/**
 * Constructor.
 */
SimulatorToolbox::SimulatorToolbox() {
}

/**
 * Returns the instance of OperationPool.
 *
 * @return The instance of OperationPool.
 */
OperationPool&
SimulatorToolbox::operationPool() {
    if (pool_ == NULL) {
        pool_ = new OperationPool();
    }
    return *pool_;
}

/**
 * Returns the instance of SimulatorTextGenerator.
 *
 * @return The instance of SimulatorTextGenerator.
 */
SimulatorTextGenerator&
SimulatorToolbox::textGenerator() {
    return textGenerator_;
}

/**
 * Clears all simulation specific data.
 *
 * This should be called before starting a new simulation.
 */
void
SimulatorToolbox::clearSimulationSpecific() {
    clearProgramErrorReports();
}

/**
 * Clears all initialized instances.
 *
 * This should be called when memory is wanted to be freed.
 */
void
SimulatorToolbox::clearAll() {
    clearSimulationSpecific();
    delete pool_;
    pool_ = NULL;
    delete eventHandler_;
    eventHandler_ = NULL;
}

/**
 * This method is used to report a runtime error detected in 
 * the simulated program.
 *
 * An SE_RUNTIME_ERROR event is announced after storing the report.
 *
 * @param eventHandler Simulation event handler for the error
 * @param severity Severity classification of the runtime error.
 * @param description Textual description of the error.
 */
void
SimulatorToolbox::reportSimulatedProgramError(
    SimulationEventHandler& eventHandler,
    RuntimeErrorSeverity severity, const std::string& description) {
    ProgramErrorDescription report;
    report.first = severity;
    report.second = description;
    programErrorReports_.push_back(report);
    eventHandler.handleEvent(SimulationEventHandler::SE_RUNTIME_ERROR);
}

/**
 * Returns a program error report with given severity and index.
 *
 * @param severity Severity.
 * @param index Index.
 * @return The error report text.
 */
std::string
SimulatorToolbox::programErrorReport(
    RuntimeErrorSeverity severity, std::size_t index) {

    size_t count = 0;
    for (ProgramErrorDescriptionList::iterator i = 
             programErrorReports_.begin(); i != programErrorReports_.end();
         ++i) {
        if ((*i).first == severity) {
            if (count == index)
                return (*i).second;
            ++count;
        }
    }
    return "";
}

/**
 * Returns the count of program error reports with given severity.
 *
 * @param severity The error report severity interested in.
 * @return The count of error reports.
 */
std::size_t 
SimulatorToolbox::programErrorReportCount(
    RuntimeErrorSeverity severity) {
    size_t count = 0;
    for (ProgramErrorDescriptionList::iterator i = 
             programErrorReports_.begin(); i != programErrorReports_.end();
         ++i) {
        if ((*i).first == severity)
            ++count;
    }
    return count;
}

/**
 * Clears the runtime error report list.
 */
void 
SimulatorToolbox::clearProgramErrorReports() {
    programErrorReports_.clear();
}

/**
 * Returns a regular expression to parse a sequential register 
 * string.
 *
 * @return The regular expression.
 */
boost::regex
SimulatorToolbox::sequentialRegisterRegex() {
    return boost::regex("([rR]|[fF])([0-9]+)");
}

/**
 * Returns a regular expression to parse a FU port string.
 *
 * @return The regular expression.
 */
boost::regex
SimulatorToolbox::fuPortRegex() {
    return boost::regex("(.*)\\.(.*)");
}
