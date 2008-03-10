/**
 * @file ProDeRFEditPolicy.cc
 *
 * Definition of ProDeRFEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <boost/format.hpp>
#include <string>

#include "ProDeRFEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "ModifyRFCmd.hh"
#include "DeleteRFCmd.hh"
#include "CopyComponent.hh"
#include "RegisterFile.hh"
#include "EditPart.hh"
#include "SetStatusTextCmd.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDeRFEditPolicy::ProDeRFEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeRFEditPolicy::~ProDeRFEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeRFEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::MODIFY_REQUEST) {
        ModifyRFCmd* modifyCmd = new ModifyRFCmd(host_);
        return modifyCmd;

    } else if (type == Request::DELETE_REQUEST) {
        DeleteRFCmd* deleteCmd = new DeleteRFCmd(host_);
        return deleteCmd;

    } else if (type == Request::COPY_REQUEST) {
        CopyComponent* copyCmd = new CopyComponent(host_);
        return copyCmd;

    } else if (type == Request::STATUS_REQUEST) {

        RegisterFile* rf = dynamic_cast<RegisterFile*>(host_->model());
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format fmt = generator->text(ProDeTextGenerator::STATUS_REGISTER_FILE);
        string type = "";
        if (rf->isNormal()) {
            type = generator->text(
                ProDeTextGenerator::TXT_RF_TYPE_NORMAL).str();
        } else if (rf->isVolatile()) {
            type = generator->text(
                ProDeTextGenerator::TXT_RF_TYPE_VOLATILE).str();
        } else if (rf->isReserved()) {
            type = generator->text(
                ProDeTextGenerator::TXT_RF_TYPE_RESERVED).str();
        } else {
            // unknown rf type
            assert(false);
        }

        fmt % rf->name() % type %
            Conversion::toString(rf->numberOfRegisters()) %
            Conversion::toString(rf->width());

        SetStatusTextCmd* statusCmd = new SetStatusTextCmd(fmt.str());
        return statusCmd;

    } else {
	return NULL;
    }
}

/**
 * Tells whether this EditPolicy is able to handle a certain type
 * of Request.
 *
 * @param request Request to be asked if it can be handled.
 * @return True if the Request can be handled, false otherwise.
 */
bool
ProDeRFEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
	type == Request::DELETE_REQUEST ||
        type == Request::COPY_REQUEST ||
        type == Request::STATUS_REQUEST) {
	return true;
    } else {
	return false;
    }
}
