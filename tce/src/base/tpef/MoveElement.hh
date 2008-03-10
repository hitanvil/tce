/**
 * @file MoveElement.hh
 *
 * Declaration of MoveElement class.
 *
 * @author Jussi Nyk�nen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepist� 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 21 October 2003 by ml, jn, rm, pj
 *
 * @note rating: yellow
 */

#ifndef TTA_MOVE_ELEMENT_HH
#define TTA_MOVE_ELEMENT_HH

#include "TPEFBaseType.hh"
#include "InstructionElement.hh"

namespace TPEF {

/**
 * Move instruction.
 */
class MoveElement : public InstructionElement {
public:
    /**
     * Type of resource that is referred by unit / index pair.
     */
    enum FieldType {
        MF_NULL, ///< Illegal field.
        MF_RF,   ///< Register file.
        MF_IMM,  ///< Immediate.
        MF_UNIT  ///< Function unit.
    };

    MoveElement();
    virtual ~MoveElement();

    bool isEmpty() const;
    void setEmpty(bool flag);

    Byte bus() const;
    void setBus(Byte aBus);

    FieldType sourceType() const;
    void setSourceType(FieldType aType);

    Byte sourceUnit() const;
    void setSourceUnit(Byte aSourceUnit);

    HalfWord sourceIndex() const;
    void setSourceIndex(HalfWord aSourceIndex);

    FieldType destinationType() const;
    void setDestinationType(FieldType aType);

    Byte destinationUnit() const;
    void setDestinationUnit(Byte aDestinationUnit);

    HalfWord destinationIndex() const;
    void setDestinationIndex(HalfWord aDestinationIndex);

    FieldType guardType() const;
    void setGuardType(FieldType gType);

    Byte guardUnit() const;
    void setGuardUnit(Byte aGuardUnit);

    HalfWord guardIndex() const;
    void setGuardIndex(HalfWord aGuardIndex);

    bool isGuarded() const;
    void setGuarded(bool flag);

    bool isGuardInverted() const;
    void setGuardInverted(bool flag);

private:
    /// Empty move bit.
    bool isEmpty_;
    /// Move bus identifier.
    Byte bus_;
    /// The type of Move source.
    FieldType sourceType_;
    /// The type of move destination.
    FieldType destinationType_;
    /// Move source unit or register file.
    Byte sourceUnit_;
    /// Move destination unit or register file identifier.
    Byte destinationUnit_;
    /// Move source register index.
    HalfWord sourceIndex_;
    /// Move destination register index.
    HalfWord destinationIndex_;
    /// Move guard enable bit.
    bool isGuarded_;
    /// Move guard inverted bit.
    bool isGuardInverted_;
    /// Guard type.
    FieldType guardType_;
    /// Guard register file / unit identifier.
    Byte guardUnit_;
    /// Guard register index.
    HalfWord guardIndex_;
};
}

#include "MoveElement.icc"

#endif
