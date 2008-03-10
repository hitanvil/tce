/**
 * @file OSEdMemoryCmd.hh
 *
 * Declaration of OSEdMemoryCmd class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_MEMORY_CMD_HH
#define TTA_OSED_MEMORY_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Class that opens memory dialog.
 */
class OSEdMemoryCmd : public GUICommand {
public:
    OSEdMemoryCmd();
    virtual ~OSEdMemoryCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdMemoryCmd(const OSEdMemoryCmd&);
    /// Assignment not allowed.
    OSEdMemoryCmd& operator=(const OSEdMemoryCmd&);
};

#endif
