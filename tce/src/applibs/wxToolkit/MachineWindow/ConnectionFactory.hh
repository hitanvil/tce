/**
 * @file ConnectionFactory.hh
 *
 * Declaration of ConnectionFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_CONNECTION_FACTORY_HH
#define TTA_CONNECTION_FACTORY_HH

#include "Factory.hh"

class EditPart;

/**
 * Factory for creating connection EditParts.
 */
class ConnectionFactory : public Factory {
public:
    ConnectionFactory();
    virtual ~ConnectionFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart*) ;
    virtual EditPart* createConnection(
	EditPart* source,
	EditPart* target) const;

private:
    /// Assignment not allowed.
    ConnectionFactory& operator=(ConnectionFactory& old);
    /// Copying not allowed.
    ConnectionFactory(ConnectionFactory& old);
};

#endif
