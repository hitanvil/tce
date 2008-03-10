/**
 * @file RedoCmd.hh
 *
 * Declaration of RedoCmd class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_REDO_CMD_HH
#define TTA_REDO_CMD_HH

#include <string>
#include "EditorCommand.hh"

/**
 * EditorCommand for redoing undone modifications to the machine.
 */
class RedoCmd : public EditorCommand {
public:
    RedoCmd();
    virtual ~RedoCmd();
    virtual bool Do();
    virtual int id() const;
    virtual RedoCmd* create() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
