/****************************************************************************/
/// @file    GNEDeleteFrame.cpp
/// @author  Pablo Alvarez Lopex
/// @date    Dec 2016
/// @version $Id$
///
// The Widget for remove network-elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEDeleteFrame.h"
#include "GNEAdditionalFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNECrossing.h"
#include "GNEPOI.h"
#include "GNEConnection.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDeleteFrame) GNEDeleteFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_ELEMENTS,   GNEDeleteFrame::onCmdSelectItem),
};

// Object implementation
FXIMPLEMENT(GNEDeleteFrame, FXScrollWindow, GNEDeleteFrameMap, ARRAYNUMBER(GNEDeleteFrameMap))

// ===========================================================================
// method definitions
// ===========================================================================
GNEDeleteFrame::GNEDeleteFrame(FXHorizontalFrame *horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Delete"),
    myMarkedAc(NULL) {
    // Create Groupbox for current element
    myGroupBoxCurrentElement = new FXGroupBox(myContentFrame, "Current and marked element", GUIDesignGroupBoxFrame);
    myCurrentElementLabel = new FXLabel(myGroupBoxCurrentElement, "- ", 0, GUIDesignLabelLeft);
    myMarkedElementLabel = new FXLabel(myGroupBoxCurrentElement, "- ", 0, GUIDesignLabelLeft);

    // Create Groupbox for current element
    myGroupBoxOptions = new FXGroupBox(myContentFrame, "Options", GUIDesignGroupBoxFrame);
    
    // Create groupbox for create crossings 
    myGroupBoxTreeList = new FXGroupBox(myContentFrame, "Childs", GUIDesignGroupBoxFrame);
    myTreelist = new FXTreeList(myGroupBoxTreeList, this, MID_CHOOSEN_ELEMENTS, FRAME_SUNKEN | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 200);
    
    // Create groupbox for help
    myGroupBoxInformation = new FXGroupBox(myContentFrame, "Information", GUIDesignGroupBoxFrame);
    myInformationLabel = new FXLabel(myGroupBoxInformation, " - Left click to delete element.\n - Hold <CTRL> and left click\n   to mark element.\n - Left click in another element\n   dismark marked element.", 0, GUIDesignLabelLeft);
}


GNEDeleteFrame::~GNEDeleteFrame() {

}


void 
GNEDeleteFrame::showAttributeCarrierChilds(GNEAttributeCarrier *ac) {
    // clear items
    myTreelist->clearItems();
    // change label
    if(ac != NULL) {
        myCurrentElementLabel->setText(("- " + toString(ac->getTag()) + " '" + ac->getID() + "'").c_str());
    } else {
        myCurrentElementLabel->setText("- ");
    }
    // check if currently there is a marked element
    if(myMarkedAc != NULL) {
        ac = myMarkedAc;
    }
    // Switch gl type of ac
    if (ac) {
        switch (dynamic_cast<GUIGlObject*>(ac)->getType()) {
            case GLO_JUNCTION: {
                GNEJunction *junction = dynamic_cast<GNEJunction*>(ac);
                // insert junction root
                FXTreeItem *junctionItem = myTreelist->insertItem(0, 0, junction->getID().c_str());
                junctionItem->setExpanded(true);
                // insert edges
                for(std::vector<GNEEdge*>::const_iterator i = junction->getGNEEdges().begin(); i != junction->getGNEEdges().end(); i++) {
                    FXTreeItem *edgeItem = myTreelist->insertItem(0, junctionItem, (*i)->getID().c_str());
                    edgeItem->setExpanded(true);
                    // insert lanes
                    for(std::vector<GNELane*>::const_iterator j = (*i)->getLanes().begin(); j != (*i)->getLanes().end(); j++) {
                        FXTreeItem *laneItem = myTreelist->insertItem(0, edgeItem, (*j)->getID().c_str());
                        laneItem->setExpanded(true);
                        // insert additionals of lanes
                        for(std::vector<GNEAdditional*>::const_iterator k = (*j)->getAdditionalChilds().begin(); k != (*j)->getAdditionalChilds().end(); k++) {
                            FXTreeItem *additionalItem = myTreelist->insertItem(0, laneItem, (*k)->getID().c_str());
                            additionalItem->setExpanded(true);
                        }
                    }
                    // insert additionals of edge
                    for(std::vector<GNEAdditional*>::const_iterator j = (*i)->getAdditionalChilds().begin(); j != (*i)->getAdditionalChilds().end(); j++) {
                        FXTreeItem *additionalItem = myTreelist->insertItem(0, edgeItem, (*j)->getID().c_str());
                        additionalItem->setExpanded(true);
                    }
                }
                // insert crossings
                for(std::vector<GNECrossing*>::const_iterator i = junction->getGNECrossings().begin(); i != junction->getGNECrossings().end(); i++) {
                    FXTreeItem *crossingItem = myTreelist->insertItem(0, junctionItem, (*i)->getID().c_str());
                    crossingItem->setExpanded(true);
                }

                break;
            }
            case GLO_EDGE: {
                GNEEdge *edge = dynamic_cast<GNEEdge*>(ac);
                // insert edge root
                FXTreeItem *edgeItem = myTreelist->insertItem(0, 0, edge->getID().c_str());
                edgeItem->setExpanded(true);
                // insert lanes
                for(std::vector<GNELane*>::const_iterator i = edge->getLanes().begin(); i != edge->getLanes().end(); i++) {
                    FXTreeItem *laneItem = myTreelist->insertItem(0, edgeItem, (*i)->getID().c_str());
                    laneItem->setExpanded(true);
                    // insert additionals of lane
                    for(std::vector<GNEAdditional*>::const_iterator j = (*i)->getAdditionalChilds().begin(); j != (*i)->getAdditionalChilds().end(); j++) {
                        FXTreeItem *additionalItem = myTreelist->insertItem(0, laneItem, (*j)->getID().c_str());
                        additionalItem->setExpanded(true);
                    }
                }
                // insert additionals of edge
                for(std::vector<GNEAdditional*>::const_iterator i = edge->getAdditionalChilds().begin(); i != edge->getAdditionalChilds().end(); i++) {
                    FXTreeItem *additionalItem = myTreelist->insertItem(0, edgeItem, (*i)->getID().c_str());
                    additionalItem->setExpanded(true);
                }
                break;
            }
            case GLO_LANE: {
                GNELane *lane = dynamic_cast<GNELane*>(ac);
                // insert edge root
                FXTreeItem *laneItem = myTreelist->insertItem(0, 0, lane->getID().c_str());
                laneItem->setExpanded(true);
                // insert additionals of lane
                for(std::vector<GNEAdditional*>::const_iterator i = lane->getAdditionalChilds().begin(); i != lane->getAdditionalChilds().end(); i++) {
                    FXTreeItem *additionalItem = myTreelist->insertItem(0, laneItem, (*i)->getID().c_str());
                    additionalItem->setExpanded(true);
                }
                break;
            }
            case GLO_POI: {

                break;
            }
            case GLO_POLYGON: {
                break;
            }
            case GLO_CROSSING: {

                break;
            }
            case GLO_ADDITIONAL: {

                break;
            }
            case GLO_CONNECTION: {

                break;
            }
            default: {
                break;
            }
        }
    }
}


void 
GNEDeleteFrame::removeAttributeCarrier(GNEAttributeCarrier *ac) {
    // check type of of GL object
    switch (dynamic_cast<GUIGlObject*>(ac)->getType()) {
        case GLO_JUNCTION: {
            myViewNet->getNet()->deleteJunction(dynamic_cast<GNEJunction*>(ac), myViewNet->getUndoList());
            break;
        }
        case GLO_EDGE: {
            myViewNet->getNet()->deleteGeometryOrEdge(dynamic_cast<GNEEdge*>(ac), myViewNet->getPositionInformation(), myViewNet->getUndoList());
            break;
        }
        case GLO_LANE: {
            myViewNet->getNet()->deleteLane(dynamic_cast<GNELane*>(ac), myViewNet->getUndoList());
            break;
        }
        case GLO_POI: {
            // XXX this is a dirty dirty hack! implemente GNEChange_POI
            myViewNet->getNet()->getShapeContainer().removePOI(dynamic_cast<GNEPOI*>(ac)->getMicrosimID());
            break;
        }
        case GLO_POLYGON: {
            //
            break;
        }
        case GLO_CROSSING: {
            myViewNet->getNet()->deleteCrossing(dynamic_cast<GNECrossing*>(ac), myViewNet->getUndoList());
            break;
        }
        case GLO_ADDITIONAL: {
            myViewNet->getViewParent()->getAdditionalFrame()->removeAdditional(dynamic_cast<GNEAdditional*>(ac));
            break;
        }
        case GLO_CONNECTION: {
            myViewNet->getNet()->deleteConnection(dynamic_cast<GNEConnection*>(ac), myViewNet->getUndoList());
            break;
        }
        default: {
            break;
        }
    }
    // update view to show changes
    myViewNet->update();
}


void 
GNEDeleteFrame::markAttributeCarrier(GNEAttributeCarrier *ac) {
    if(ac != NULL) {
        myMarkedAc = ac;
        myMarkedElementLabel->setText(("- " + toString(myMarkedAc->getTag()) + " '" + myMarkedAc->getID() + "'").c_str());
        showAttributeCarrierChilds(ac);
    } else {
        myMarkedAc = NULL;
        myMarkedElementLabel->setText("- ");
    }
}


GNEAttributeCarrier*
GNEDeleteFrame::getMarkedAttributeCarrier() const {
    return myMarkedAc;
}

long 
GNEDeleteFrame::onCmdSelectItem(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
