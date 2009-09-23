/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file LLVMBackend.hh
 *
 * Declaration of LLVMBackend class.
 *
 * @author Veli-Pekka Jääskeläinen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef LLVM_TCE_HH
#define LLVM_TCE_HH

#include <string>
#include "Exception.hh"
#include "PluginTools.hh"
#include "SchedulingPlan.hh"

namespace TTAProgram {
    class Program;
}

namespace TTAMachine {
    class Machine;
}

namespace llvm {
    class Module;
    class TCETargetMachinePlugin;
}

class InterPassData;


/**
 * LLVM compiler backend and compiler interface for TCE.
 */
class LLVMBackend {
public:

    LLVMBackend(bool useCache = true, bool useInstalledVersion = false, bool removeTempFiles = true);
    virtual ~LLVMBackend();

    TTAProgram::Program* compile(
        const std::string& bytecodeFile,
        const std::string& emulationBytecodeFile,
        TTAMachine::Machine& target,
        int optLevel,
        bool debug = false,
        InterPassData* ipData = NULL)
        throw (Exception);

    TTAProgram::Program* compile(
        llvm::Module& module,
        llvm::Module* emulationModule,
        llvm::TCETargetMachinePlugin& plugin,
        TTAMachine::Machine& target,
        int optLevel,
        bool debug = false,
        InterPassData* ipData = NULL)
        throw (Exception);

    TTAProgram::Program* compileAndSchedule(
        const std::string& bytecodeFile,
        const TTAMachine::Machine& target,
        int optLevel = 2,
        const unsigned int debug = 0)
        throw (Exception);

    TTAProgram::Program* schedule(
        const std::string& bytecodeFile,
        const std::string& emulationBytecodeFile,
        TTAMachine::Machine& target,
        int optLevel = 2,
        bool debug = false,
        SchedulingPlan* plan = NULL)
        throw (Exception);

    llvm::TCETargetMachinePlugin* createPlugin(
        const TTAMachine::Machine& target)
        throw (Exception);

private:

    std::string pluginFilename(const TTAMachine::Machine& target);

    /// Plugin tool for loading target machine plugin.
    PluginTools pluginTool_;
    /// Path to the cache where precompiled plugins are stored.
    std::string cachePath_;

    bool useCache_;
    bool useInstalledVersion_;
    bool removeTmp_;
    InterPassData* ipData_;

    static const std::string TCEPLUGINGEN_BIN;
    static const std::string TBLGEN_INCLUDES;
    static const std::string LLVM_CXX_FLAGS;
    static const std::string LLVM_LD_FLAGS;
    static const std::string LLVM_INCLUDE_DIR;

    static const std::string PLUGIN_PREFIX;
    static const std::string PLUGIN_SUFFIX;

};

#endif
