/**
 * @file CmdLineOptionParser.hh
 *
 * Declaration of CmdLineOptionParser classes.
 *
 * @author Jussi Nyk�nen 2003 (nykanen@cs.tut.fi)
 * @author Jari M�ntyneva 2006 (jari.mantyneva@tut.fi)
 * @note reviewed 3 December 2003 by jn, kl, ao
 */

#ifndef TTA_CMD_LINE_OPTION_PARSER_HH
#define TTA_CMD_LINE_OPTION_PARSER_HH

#include <string>
#include <vector>
#include "Exception.hh"
#include "OptionValue.hh"

//////////////////////////////////////////////////////////////////////////////
// CmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for modeling command line options and their built-in
 * mini-parser.
 *
 * All option types have a set of properties that identify the option: name,
 * description and short name (alias). Possible concrete types of options
 * are: Boolean, real, integer, and character string.
 */
class CmdLineOptionParser {
public:

    CmdLineOptionParser(std::string name, std::string desc, std::string alias);
    virtual ~CmdLineOptionParser();

    virtual OptionValue* copy() const = 0;

    std::string longName() const;
    std::string shortName() const;
    std::string description() const;

    /// Pure virtual function that parses the value of option.
    virtual bool parseValue(std::string arguments, std::string prefix)
        throw (IllegalCommandLine) = 0;

    bool isDefined();

    virtual int integer(int index = 0) const
	throw (WrongSubclass);
    virtual std::string String(int index = 0) const
        throw (WrongSubclass);
    virtual double real() const
	throw (WrongSubclass);
    virtual bool isFlagOn() const
	throw (WrongSubclass);
    virtual bool isFlagOff() const
	throw (WrongSubclass);
    virtual int listSize() const
	throw (WrongSubclass);

protected:

    void setDefined();

private:

    /// Copying not allowed.
    CmdLineOptionParser(const CmdLineOptionParser&);
    /// Assignment not allowed.
    CmdLineOptionParser& operator=(const CmdLineOptionParser&);

    /// The full name of an option.
    std::string longName_;
    /// The optional alias (shorter name).
    std::string shortName_;
    /// The description of option.
    std::string desc_;

    /// Is the value of this option set in the parsed command line?
    bool defined_;
};


//////////////////////////////////////////////////////////////////////////////
// IntegerCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Models an option that has an integer value.
 */
class IntegerCmdLineOptionParser : public CmdLineOptionParser {
public:
    IntegerCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~IntegerCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix)
        throw (IllegalCommandLine);
    virtual int integer(int index = 0) const
	throw (WrongSubclass);

private:
    /// Copying not allowed.
    IntegerCmdLineOptionParser(const IntegerCmdLineOptionParser&);
    /// Assignment not allowed.
    IntegerCmdLineOptionParser& operator=(const IntegerCmdLineOptionParser&);

    /// The value of option.
    int value_;
};

//////////////////////////////////////////////////////////////////////////////
// StringCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a string as a value.
 */
class StringCmdLineOptionParser : public CmdLineOptionParser {
public:
    StringCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~StringCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix)
        throw (IllegalCommandLine);
    virtual std::string String(int index = 0) const
	throw (WrongSubclass);

private:
    /// Copying not allowed.
    StringCmdLineOptionParser(const StringCmdLineOptionParser&);
    /// Assignment not allowed.
    StringCmdLineOptionParser& operator=(const StringCmdLineOptionParser&);

    /// The value of the option.
    std::string value_;
};

//////////////////////////////////////////////////////////////////////////////
// RealCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a real value.
 */
class RealCmdLineOptionParser : public CmdLineOptionParser {
public:
    RealCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~RealCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix)
        throw (IllegalCommandLine);
    virtual double real() const
	throw (WrongSubclass);

private:
    /// Copying not allowed.
    RealCmdLineOptionParser(const RealCmdLineOptionParser&);
    /// Assignment not allowed.
    RealCmdLineOptionParser& operator=(const RealCmdLineOptionParser&);

    /// The value of the option.
    double value_;
};

//////////////////////////////////////////////////////////////////////////////
// BoolCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a boolean value.
 *
 * This option is also called 'flag'.
 */
class BoolCmdLineOptionParser : public CmdLineOptionParser {
public:
    BoolCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~BoolCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix)
        throw (IllegalCommandLine);
    virtual bool isFlagOn() const
        throw (WrongSubclass);
    virtual bool isFlagOff() const
        throw (WrongSubclass);

private:
    /// Copying not allowed.
    BoolCmdLineOptionParser(const BoolCmdLineOptionParser&);
    /// Assignment not allowed.
    BoolCmdLineOptionParser& operator=(const BoolCmdLineOptionParser&);

    /// The value of option.
    bool value_;
};

//////////////////////////////////////////////////////////////////////////////
// IntegerListCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a list of integers as value.
 */
class IntegerListCmdLineOptionParser : public CmdLineOptionParser {
public:
    IntegerListCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~IntegerListCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix)
        throw (IllegalCommandLine);

    virtual int integer(int index = 0) const
	throw (WrongSubclass);
    virtual int listSize() const
	throw (WrongSubclass);


private:
    /// Copying not allowed.
    IntegerListCmdLineOptionParser(const IntegerListCmdLineOptionParser&);
    /// Assignment not allowed.
    IntegerListCmdLineOptionParser&
    operator=(const IntegerListCmdLineOptionParser&);

    /// The values in integer list.
    std::vector<int> values_;
};


//////////////////////////////////////////////////////////////////////////////
// StringListCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a list of strings as value.
 */
class StringListCmdLineOptionParser : public CmdLineOptionParser {
public:
    StringListCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~StringListCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix)
        throw (IllegalCommandLine);

    virtual std::string String(int index = 0) const
        throw (WrongSubclass);
    virtual int listSize() const
        throw (WrongSubclass);


private:
    /// Copying not allowed.
    StringListCmdLineOptionParser(const StringListCmdLineOptionParser&);
    /// Assignment not allowed.
    StringListCmdLineOptionParser&
    operator=(const StringListCmdLineOptionParser&);

    /// The values in string list.
    std::vector<std::string> values_;
};

#include "CmdLineOptionParser.icc"

#endif
