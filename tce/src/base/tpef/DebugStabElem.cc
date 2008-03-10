/**
 * @file DebugStabElem.cc
 *
 * Non-inline definitions of DebugStabElem class.
 *
 * @author Mikael Lepist� 2006 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "DebugStabElem.hh"

namespace TPEF {

/**
 * Constructor.
 *
 * Creates debug element out of data vector, where all the data
 * is stored in big endian format. 
 *
 * This constructor is used when additional data part of debug 
 * element is read from data is read from TPEF file to table.
 *
 * @param data Data vector that is used to initialize a.out stab element.
 */
DebugStabElem::DebugStabElem(std::vector<Byte>& data)
    : DebugElement() {

    // read element to object
    stabType_ = data[0];
    other_ = data[1];
    description_ = data[2];
    description_ = (description_ << BYTE_BITWIDTH) | data[3];
    value_ = data[4];
    value_ = (value_ << BYTE_BITWIDTH) | data[5];
    value_ = (value_ << BYTE_BITWIDTH) | data[6];
    value_ = (value_ << BYTE_BITWIDTH) | data[7];
}


/**
 * Constructor.
 *
 * @param stabType A.out stab's type field.
 * @param other A.out stab's other field.
 * @param desscription A.out stab's description field.
 * @param value A.out stab's value field.
 */
DebugStabElem::DebugStabElem(
    Byte stabType, Byte other, HalfWord description, Word value) :
    DebugElement(), stabType_(stabType), other_(other),
    description_(description), value_(value) {
}


/**
 * Destructor.
 */
DebugStabElem::~DebugStabElem() {
}


/**
 * Returns one byte of debug element's additional data field.
 *
 * @param index Which data byte is wanted.
 */
Byte
DebugStabElem::byte(Word index) const throw (OutOfRange) {
    switch (index) {
    case 0:
        return stabType_;
    case 1:
        return other_;
    case 2:
        return description_ >> BYTE_BITWIDTH;
    case 3:
        return description_;
    case 4:
        return value_ >> (3*BYTE_BITWIDTH);
    case 5:
        return value_ >> (2*BYTE_BITWIDTH);
    case 6:
        return value_ >> BYTE_BITWIDTH;
    case 7:
        return value_;
    default:
        throw OutOfRange(__FILE__, __LINE__, __func__,
                         "Stab element contains less data that requested.");
    }
}


/**
 * Returns additional data length of TPEF debug element.
 *
 * TPEF debug element for sotring a.out stab symbols contains
 * always 8 bytes of additional data. This element is described
 * in TPEF specification.
 *
 * @return Number of additional data bytes that are stored.
 */
Word
DebugStabElem::length() const {
    return 8;
}


/**
 * Returns type of TPEF debug element.
 *
 * @return Type of TPEF debug element.
 */
DebugStabElem::ElementType
DebugStabElem::type() const {
    return DE_STAB;
}


/**
 * Returns type of stored a.out stab symbol.
 *
 * @return Type of stored a.out stab symbol.
 */
Byte
DebugStabElem::stabType() const {
    return stabType_;
}


/**
 * Returns other field of stored a.out stab symbol.
 *
 * @return Other field of stored a.out stab symbol.
 */
Byte
DebugStabElem::other() const {
    return other_;
}


/**
 * Returns description field of stored a.out stab symbol.
 *
 * @return Description field of stored a.out stab symbol.
 */
HalfWord
DebugStabElem::description() const {
    return description_;
}


/**
 * Returns value field of stored a.out stab symbol.
 *
 * @return Value field of stored a.out stab symbol.
 */
Word
DebugStabElem::value() const {
    return value_;
}

} // namespace TPEF
