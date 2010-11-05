/*
    Copyright (c) 2002-2010 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file SOPCBuilderFileGenerator.cc
 *
 * Implementation of SOPCBuilderFileGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <fstream>
#include <cassert>
#include "SOPCBuilderFileGenerator.hh"
#include "PlatformIntegrator.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "Exception.hh"
#include "FileSystem.hh"
#include "AvalonMMMasterInterface.hh"
#include "MapTools.hh"
using std::ofstream;
using std::string;
using std::vector;
using std::map;
using std::endl;


const std::string SOPCBuilderFileGenerator::HDB_AVALON_PREFIX = "avalon_d_";
const std::string SOPCBuilderFileGenerator::SOPC_COMPONENT_FILE_TYPE =
    "_hw.tcl";
const std::string SOPCBuilderFileGenerator::SOPC_SET_MODULE_PROPERTY =
    "set_module_property";
const std::string SOPCBuilderFileGenerator::SOPC_ADD_FILE =
    "add_file";
const std::string SOPCBuilderFileGenerator::TTA_CLOCK_NAME = "clk";
const std::string SOPCBuilderFileGenerator::TTA_RESET_NAME = "rstx";
const std::string SOPCBuilderFileGenerator::SOPC_CLOCK_NAME = "clk";
const std::string SOPCBuilderFileGenerator::SOPC_RESET_NAME = "reset_n";
const std::string SOPCBuilderFileGenerator::SOPC_DEFAULT_GROUP = "User Logic";
const std::string SOPCBuilderFileGenerator::SOPC_DEFAULT_VHDL_LIBS = 
    "std.standard.all ieee.std_logic_1164.all ieee.std_logic_arith.all "
    "work.globals.all work.util.all work.imem_mau.all";





SOPCBuilderFileGenerator::SOPCBuilderFileGenerator(
    std::string toplevelEntity,
    const PlatformIntegrator* integrator):
    ProjectFileGenerator(toplevelEntity, integrator),
    clock_(new SOPCInterface(
               SOPCInterface::SOPC_CLOCK_INT_NAME,
               SOPCInterface::SOPC_CLOCK_INT_DECLR)),
    export_(new SOPCInterface(
                SOPCInterface::SOPC_EXPORT_INT_NAME,
                SOPCInterface::SOPC_EXPORT_INT_DECLR)) {
    
    clock_->setProperty("ptfSchematicName", "\"\"");
    clock_->setPort(TTA_CLOCK_NAME, SOPC_CLOCK_NAME, HDB::IN, 1);
    clock_->setPort(TTA_RESET_NAME, SOPC_RESET_NAME, HDB::IN, 1);
    
    export_->setProperty(
        SOPCInterface::SOPC_ASSOCIATED_CLOCK, clock_->name());
}


SOPCBuilderFileGenerator::~SOPCBuilderFileGenerator() {
    
    if (clock_ != NULL) {
        delete clock_;
    }
    if (export_ != NULL) {
        delete export_;
    }
    MapTools::deleteAllValues(masters_);
}


void
SOPCBuilderFileGenerator::writeProjectFiles() {
    
    int masterInterfaces = countAvalonMMMasters();
    if (masterInterfaces == 0) {
        string msg = "Couldn't find any Avalon MM Master interfaces.";
        InvalidData exc(__FILE__, __LINE__, "SOPCBuilderFileGenerator", msg);
        throw exc;
    }

    createInterfaces();

    string outputFileName = toplevelEntity() + SOPC_COMPONENT_FILE_TYPE;
    ofstream output;
    output.open(outputFileName.c_str());
    if (!output) {
        string msg = "Couldn't open file " + outputFileName + " for writing";
        IOException exc(__FILE__, __LINE__, "SOPCBuilderFileGenerator", msg);
        throw exc;
    }
    
    output << "# Generated by SOPCBuilderGenerator" << endl << endl;
    writeModuleProperties(output);

    writeFileList(output);

    writeInterfaces(output);

    output.close();
}


/**
 * Counts how many Avalon MM Master interfaces toplevel netlis block has.
 *
 * Avalon MM interface is recognised by the address port. Address signal in
 * first of the 2 mandatory signals in Avalon MM interface. Direction of the
 * address port determines whether it belongs to a master or slave interface.
 *
 * @return Number of found Avalon MM Master interfaces
 */
int
SOPCBuilderFileGenerator::countAvalonMMMasters() const {
    
    const ProGe::NetlistBlock& top = integrator()->toplevelBlock();
    string addressPortName =
        HDB_AVALON_PREFIX + AvalonMMMasterInterface::AVALON_MM_ADDRESS;
    int found = 0;
    for (int i = 0; i < top.portCount(); i++) {
        string portName = top.port(i).name();
        if (portName.find(addressPortName) != string::npos) {
            if (top.port(i).direction() == HDB::OUT) {
                found++;
            }
        }
    }
    return found;
}


void
SOPCBuilderFileGenerator::writeModuleProperties(std::ostream& stream) {

    string property = SOPC_SET_MODULE_PROPERTY;
    stream 
        << "# Module properties" << endl
        << property << " NAME " << toplevelEntity() << endl
        << property << " VERSION 1.0" << endl
        << property << " GROUP \"" << SOPC_DEFAULT_GROUP << "\"" << endl
        << property << " DISPLAY_NAME " << toplevelEntity() << endl
        << property << " LIBRARIES {" << SOPC_DEFAULT_VHDL_LIBS;

    if (integrator()->toplevelBlock().netlist().parameterCount() > 0) {
        stream << " work." << toplevelEntity() << "_params.all";
    }
    stream << "}" << endl;

    string toplevelFile =
        integrator()->outputFilePath(toplevelEntity() + ".vhdl");
    stream
        << property << " TOP_LEVEL_HDL_FILE " << toplevelFile << endl
        << property << " TOP_LEVEL_HDL_MODULE " << toplevelEntity() << endl
        << property << " INSTANTIATE_IN_SYSTEM_MODULE true" << endl
        << property << " EDITABLE false" << endl
        << property << " SIMULATION_MODEL_IN_VERILOG false" << endl
        << property << " SIMULATION_MODEL_IN_VHDL false" << endl
        << property << " SIMULATION_MODEL_HAS_TULIPS false" << endl
        << property << " SIMULATION_MODEL_IS_OBFUSCATED false" << endl
        << endl;
}


void
SOPCBuilderFileGenerator::writeFileList(std::ostream& stream) {

    stream << "# module hdl files" << endl;
    for (unsigned int i = 0; i < hdlFileList().size(); i++) {
        stream << SOPC_ADD_FILE << " " << hdlFileList().at(i)
               << " {SYNTHESIS SIMULATION}" << endl;
    }
    for (unsigned int i = 0; i < memInitFileList().size(); i++) {
        stream << SOPC_ADD_FILE << " " << memInitFileList().at(i)
               << " {SYNTHESIS SIMULATION}" << endl;
    }
    stream << endl;
}


void
SOPCBuilderFileGenerator::createInterfaces() {

    const ProGe::NetlistBlock& top = integrator()->toplevelBlock();
    for (int i = 0; i < top.portCount(); i++) {
        string portName = top.port(i).name();

        bool needToExport = true;
        if (portName == TTA_CLOCK_NAME || portName == TTA_RESET_NAME) {
            continue;
        } else if (portName.find(HDB_AVALON_PREFIX) != string::npos) {
            needToExport = !handleAvalonSignal(top.port(i));
        }

        if (needToExport) {
            exportSignal(top.port(i));
        }
    }
}


bool
SOPCBuilderFileGenerator::handleAvalonSignal(
    const ProGe::NetlistPort& port) {

    string fuName = extractFUName(port.name(), HDB_AVALON_PREFIX);

    if (fuName.empty()) {
        string msg = "Failed to extract FU name from: " + port.name();
        InvalidData exc(__FILE__, __LINE__, "SOPCBuilderFileGenerator", msg);
        throw exc;
    }
    
    if (!MapTools::containsKey(masters_ ,fuName)) {
        string interfaceName = fuName + SOPCInterface::SOPC_MASTER_INT_NAME;
        string declr = SOPCInterface::SOPC_MASTER_INT_DECLR;
        masters_[fuName] = new AvalonMMMasterInterface(
            interfaceName, declr, HDB_AVALON_PREFIX, *clock_);
    }

    AvalonMMMasterInterface* master = getMaster(fuName);
    bool isAvalonPort = false;
    if (!master->isValidPort(port)) {
        isAvalonPort = false;
    } else {
        master->addPort(port);
        isAvalonPort = true;
    }
    return isAvalonPort;
}


void
SOPCBuilderFileGenerator::exportSignal(const ProGe::NetlistPort& port) {

    int width = 32;
    if (port.realWidthAvailable()) {
        width = port.realWidth();
    }
    string intPortName = SOPCInterface::SOPC_EXPORT_NAME;
    export_->setPort(port.name(), intPortName, port.direction(), width);
}


void
SOPCBuilderFileGenerator::writeInterfaces(std::ostream& stream) const {

    clock_->writeInterface(stream);

    std::map<std::string, AvalonMMMasterInterface*>::const_iterator iter =
        masters_.begin();
    while (iter != masters_.end()) {
        AvalonMMMasterInterface* master = iter->second;
        if (master->hasPorts()) {
            master->writeInterface(stream);
        }
        iter++;
    }
    if (export_->hasPorts()) {
        export_->writeInterface(stream);
    }
}





AvalonMMMasterInterface*
SOPCBuilderFileGenerator::getMaster(const std::string& fuName) {

    std::map<std::string, AvalonMMMasterInterface*>::iterator iter =
        masters_.find(fuName);

    assert(iter != masters_.end());
    return iter->second;
}