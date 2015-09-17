/****************************************************************************/
/// @file    MSSOTLE2Sensors.cpp
/// @author  Gianfilippo Slager
/// @author  Alessio Bonfietti
/// @date    Feb 2010
/// @version $Id: MSSOTLE2Sensors.cpp 3 2010-03-03 15:00:00Z gslager $
///
// The class for SOTL sensors of "E2" type
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include "MSSOTLE2Sensors.h"
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSEdge.h>

MSSOTLE2Sensors::MSSOTLE2Sensors(std::string tlLogicID,
		const MSTrafficLightLogic::Phases *phases) :
		MSSOTLSensors(tlLogicID, phases) {
}

MSSOTLE2Sensors::~MSSOTLE2Sensors(void)
{
  //Delete sensors
  for (MSLaneID_MSE2CollectorMap::iterator sensorsIterator = m_sensorMap.begin(); sensorsIterator != m_sensorMap.end();
      sensorsIterator++)
  {
    delete (sensorsIterator->second);
  }
}

void MSSOTLE2Sensors::buildSensors(
		MSTrafficLightLogic::LaneVectorVector controlledLanes,
		NLDetectorBuilder &nb){
	buildSensors(controlledLanes,nb,INPUT_SENSOR_LENGTH);
}

void MSSOTLE2Sensors::buildSensors(
		MSTrafficLightLogic::LaneVectorVector controlledLanes,
		NLDetectorBuilder &nb, double sensorLength) {
	//for each lane build an appropriate sensor on it
	MSLane *currentLane = NULL;

	//input and ouput lanes
	for (MSTrafficLightLogic::LaneVectorVector::const_iterator laneVector =
			controlledLanes.begin(); laneVector != controlledLanes.end();
			laneVector++) {
		for (MSTrafficLightLogic::LaneVector::const_iterator lane =
				laneVector->begin(); lane != laneVector->end(); lane++) {
			currentLane = (*lane);
			buildSensorForLane(currentLane, nb,sensorLength);
		}
	}
}
/****************************************************************************/
/*
 * Count Sensors. Should be refactor to make a new class.
 */
void MSSOTLE2Sensors::buildCountSensors(
		MSTrafficLightLogic::LaneVectorVector controlledLanes,
		NLDetectorBuilder &nb) {
	//for each lane build an appropriate sensor on it
	MSLane *currentLane = NULL;
	//input and ouput lanes
	for (MSTrafficLightLogic::LaneVectorVector::const_iterator laneVector =
			controlledLanes.begin(); laneVector != controlledLanes.end();
			laneVector++) {
		for (MSTrafficLightLogic::LaneVector::const_iterator lane =
				laneVector->begin(); lane != laneVector->end(); lane++) {
			currentLane = (*lane);
			buildCountSensorForLane(currentLane, nb);
		}
	}
}

void MSSOTLE2Sensors::buildCountOutSensors(
		MSTrafficLightLogic::LaneVectorVector controlledLanes,
		NLDetectorBuilder &nb) {
	//for each lane build an appropriate sensor on it
	MSLane *currentLane = NULL;

	//input and ouput lanes
	for (MSTrafficLightLogic::LaneVectorVector::const_iterator laneVector =
			controlledLanes.begin(); laneVector != controlledLanes.end();
			laneVector++) {
		for (MSTrafficLightLogic::LaneVector::const_iterator lane =
				laneVector->begin(); lane != laneVector->end(); lane++) {
			currentLane = (*lane);
			buildCountSensorForOutLane(currentLane, nb);
		}
	}
}

void MSSOTLE2Sensors::buildOutSensors(
		MSTrafficLightLogic::LaneVectorVector controlledLanes,
		NLDetectorBuilder &nb){
	buildOutSensors(controlledLanes,nb,OUTPUT_SENSOR_LENGTH);
}
void MSSOTLE2Sensors::buildOutSensors(
		MSTrafficLightLogic::LaneVectorVector controlledLanes,
		NLDetectorBuilder &nb, double sensorLength) {
	//for each lane build an appropriate sensor on it
	MSLane *currentLane = NULL;

	//input and ouput lanes
	for (MSTrafficLightLogic::LaneVectorVector::const_iterator laneVector =
			controlledLanes.begin(); laneVector != controlledLanes.end();
			laneVector++) {
		for (MSTrafficLightLogic::LaneVector::const_iterator lane =
				laneVector->begin(); lane != laneVector->end(); lane++) {
			currentLane = (*lane);
			buildSensorForOutLane(currentLane, nb, sensorLength);
		}
	}
}

void MSSOTLE2Sensors::buildCountSensorForLane(MSLane* lane, NLDetectorBuilder &nb) {
	float sensorPos;
	double lensorLength;
	MSE2Collector *newSensor = NULL;
	//Check not to have more than a sensor for lane
	if (m_sensorMap.find(lane->getID()) == m_sensorMap.end()) {

		//Check and set zero if the lane is not long enough for the specified sensor start
		sensorPos = COUNT_SENSOR_START <= lane->getLength() ? COUNT_SENSOR_START : 0;

		//Original:
		double sensorLength = INPUT_COUNT_SENSOR_LENGTH;

		//Check and trim if the lane is not long enough for the specified sensor length
		lensorLength = sensorLength <= (lane->getLength() - sensorPos) ? sensorLength : (lane->getLength() - sensorPos);

		//TODO check this lengths
		DBG(
		std::ostringstream phero_str;
		phero_str << " lane " << lane->getID() << " sensorPos= " << sensorPos
				<< " ,SENSOR_START  " << SENSOR_START << "; lane->getLength = "
				<< lane->getLength() << " ,lensorLength= " << lensorLength
				<< " ,SENSOR_LENGTH= " << INPUT_SENSOR_LENGTH;
		WRITE_MESSAGE(
				"MSSOTLE2Sensors::buildSensorForLane::" + phero_str.str());
		)

		//Create sensor for lane and insert it into the map<MSLane*, MSE2Collector*>
		newSensor = nb.buildSingleLaneE2Det(
				"SOTL_E2_lane:" + lane->getID() + "_tl:" + tlLogicID,
				DU_TL_CONTROL, lane,
				(lane->getLength() - sensorPos - lensorLength), lensorLength,
				HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);
		//newSensor = nb.buildSingleLaneE2Det("SOTL_E2_lane:"+lane->getID()+"_tl:"+tlLogicID, DU_TL_CONTROL, lane, (lane->getLength() - sensorPos- 5), lensorLength, HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);

		MSNet::getInstance()->getDetectorControl().add(
				SUMO_TAG_LANE_AREA_DETECTOR, newSensor);

		m_sensorMap.insert(MSLaneID_MSE2Collector(lane->getID(), newSensor));
		m_maxSpeedMap.insert(MSLaneID_MaxSpeed(lane->getID(), lane->getSpeedLimit()));
	}
}

void MSSOTLE2Sensors::buildCountSensorForOutLane(MSLane* lane,
		NLDetectorBuilder &nb) {
	float sensorPos;
	double lensorLength;
	MSE2Collector *newSensor = NULL;
	//Check not to have more than a sensor for lane
	if (m_sensorMap.find(lane->getID()) == m_sensorMap.end()) {

		//Original:
		double sensorLength = OUTPUT_COUNT_SENSOR_LENGTH;
		//Check and set zero if the lane is not long enough for the specified sensor start
		sensorPos = (lane->getLength() - sensorLength)
						- (SENSOR_START <= lane->getLength() ? SENSOR_START : 0);

		//Check and trim if the lane is not long enough for the specified sensor lenght
		lensorLength =
				sensorLength <= (lane->getLength() - sensorPos) ?
						sensorLength : (lane->getLength() - sensorPos);

		//TODO check this lengths
		DBG(
		std::ostringstream phero_str;
		phero_str << " lane " << lane->getID() << " sensorPos= " << sensorPos
				<< " ,SENSOR_START  " << SENSOR_START << "; lane->getLength = "
				<< lane->getLength() << " ,lensorLength= " << lensorLength
				<< " ,SENSOR_LENGTH= " << INPUT_SENSOR_LENGTH;
		WRITE_MESSAGE(
				"MSSOTLE2Sensors::buildSensorForLane::" + phero_str.str());
		)

		//Create sensor for lane and insert it into the map<MSLane*, MSE2Collector*>
		newSensor = nb.buildSingleLaneE2Det(
				"SOTL_E2_lane:" + lane->getID() + "_tl:" + tlLogicID,
				DU_TL_CONTROL, lane,
				(lane->getLength() - sensorPos - lensorLength), lensorLength,
				HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);
		//newSensor = nb.buildSingleLaneE2Det("SOTL_E2_lane:"+lane->getID()+"_tl:"+tlLogicID, DU_TL_CONTROL, lane, (lane->getLength() - sensorPos- 5), lensorLength, HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);

		MSNet::getInstance()->getDetectorControl().add(
				SUMO_TAG_LANE_AREA_DETECTOR, newSensor);

		m_sensorMap.insert(MSLaneID_MSE2Collector(lane->getID(), newSensor));
		m_maxSpeedMap.insert(MSLaneID_MaxSpeed(lane->getID(), lane->getSpeedLimit()));
	}
}

unsigned int MSSOTLE2Sensors::getPassedVeh(std::string laneId, bool out)
{
  MSLaneID_MSE2CollectorMap::const_iterator sensorsIterator;
  sensorsIterator = m_sensorMap.find(laneId);
  if (sensorsIterator == m_sensorMap.end())
  {
    assert(0);
    return 0;
  } else
  {
    unsigned int additional = 0;
    if(m_continueSensorOnLanes.find(laneId) != m_continueSensorOnLanes.end())
      for(std::vector<std::string>::iterator it = m_continueSensorOnLanes[laneId].begin(); it != m_continueSensorOnLanes[laneId].end(); ++ it)
      {
        unsigned int tmp = 0;
        if(getVelueFromSensor(*it, &MSE2Collector::getPassedVeh, tmp))
          additional += tmp;
      }
    return sensorsIterator->second->getPassedVeh() + additional;
  }
}

void
MSSOTLE2Sensors::subtractPassedVeh(std::string laneId, int passed){
	//TODO Ivan: ciclare tutti i sensori di count e resettare passedVeh
	MSLaneID_MSE2CollectorMap::const_iterator sensorsIterator;

	sensorsIterator = m_sensorMap.find(laneId);
	if (sensorsIterator != m_sensorMap.end())
		sensorsIterator->second->subtractPassedVeh(passed);
}

/*******************************************************************************/
/* @brief Builds an e2 detector that lies on only one lane
 *
 * @param[in] id The id the detector shall have
 * @param[in] usage Information how the detector is used within the simulation
 * @param[in] lane The lane the detector is placed at
 * @param[in] pos The position on the lane the detector is placed at
 * @param[in] length The length the detector has
 * @param[in] haltingTimeThreshold Detector parameter: the time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
 * @param[in] haltingSpeedThreshold Detector parameter: the speed a vehicle's speed must be below to be assigned as jammed
 * @param[in] jamDistThreshold Detector parameter: the distance between two vehicles in order to not count them to one jam
 * @todo Check whether this method is really needful
 */
/*
 Using
 MSE2Collector* buildSingleLaneE2Det(const std::string& id,
 DetectorUsage usage, MSLane* lane, SUMOReal pos, SUMOReal length,
 SUMOTime haltingTimeThreshold, SUMOReal haltingSpeedThreshold,
 SUMOReal jamDistThreshold);

 */

void MSSOTLE2Sensors::buildSensorForLane(MSLane* lane, NLDetectorBuilder &nb){
	buildSensorForLane(lane,nb,INPUT_SENSOR_LENGTH);
}



void MSSOTLE2Sensors::buildSensorForLane(MSLane* lane, NLDetectorBuilder &nb, double sensorLength) {
  const MSEdge* edge = &lane->getEdge();
  if(edge->isInternal() || edge->isWalkingArea() || edge->isCrossing())
    return;
	float sensorPos;
	double lensorLength;
	MSE2Collector *newSensor = NULL;
	//Check not to have more than a sensor for lane
	if (m_sensorMap.find(lane->getID()) == m_sensorMap.end()) {

		//Check and set zero if the lane is not long enough for the specified sensor start
		sensorPos = SENSOR_START <= lane->getLength() ? SENSOR_START : 0;
		//Check and trim if the lane is not long enough for the specified sensor lenght
		lensorLength = sensorLength <= (lane->getLength() - sensorPos) ? sensorLength : (lane->getLength() - sensorPos);
		//TODO check this lengths
		DBG(
		std::ostringstream phero_str;
		phero_str << " lane " << lane->getID() << " sensorPos= " << sensorPos
				<< " ,SENSOR_START  " << SENSOR_START << "; lane->getLength = "
				<< lane->getLength() << " ,lensorLength= " << lensorLength
				<< " ,SENSOR_LENGTH= " << INPUT_SENSOR_LENGTH;
		WRITE_MESSAGE(
				"MSSOTLE2Sensors::buildSensorForLane::" + phero_str.str());
		)

		//Create sensor for lane and insert it into the map<MSLane*, MSE2Collector*>
		newSensor = nb.buildSingleLaneE2Det(
				"SOTL_E2_lane:" + lane->getID() + "_tl:" + tlLogicID,
				DU_TL_CONTROL, lane,
				(lane->getLength() - sensorPos - lensorLength), lensorLength,
				HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);
//newSensor = nb.buildSingleLaneE2Det("SOTL_E2_lane:"+lane->getID()+"_tl:"+tlLogicID, DU_TL_CONTROL, lane, (lane->getLength() - sensorPos- 5), lensorLength, HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);

		MSNet::getInstance()->getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, newSensor);

		m_sensorMap.insert(MSLaneID_MSE2Collector(lane->getID(), newSensor));
		m_maxSpeedMap.insert(MSLaneID_MaxSpeed(lane->getID(), lane->getSpeedLimit()));
		//Avoid creating really small sensors. Only add a further sensors if the one created is less than 90% the requested length
		if(lensorLength < sensorLength * 0.9)
		{
		  std::ostringstream oss;
		  oss << "Sensor on lane " << lane->getID() << " is long " << lensorLength << ", while it should be " << sensorLength << ". Continuing it on the other lanes if possible";
		  WRITE_MESSAGE(oss.str())
		  for(std::vector<MSLane::IncomingLaneInfo>::const_iterator it = lane->getIncomingLanes().begin(); it != lane->getIncomingLanes().end(); ++it)
		  {
		    const MSEdge* edge = &it->lane->getEdge();
		    if(!edge->isInternal() && !edge->isWalkingArea() && !edge->isCrossing())
		      buildContinueSensior(lane, nb, sensorLength, it->lane, lensorLength);
		  }
		}
	}
}

void MSSOTLE2Sensors::buildContinueSensior(MSLane* lane, NLDetectorBuilder &nb, double sensorLength, MSLane* continueOnLane, double usedLength)
{
  double availableLength = sensorLength - usedLength;
  if (m_sensorMap.find(continueOnLane->getID()) == m_sensorMap.end())
  {
    double length = availableLength <= continueOnLane->getLength() ? availableLength : continueOnLane->getLength();
    MSE2Collector *newSensor = nb.buildSingleLaneE2Det(
        "SOTL_E2_lane:" + continueOnLane->getID() + "_tl:" + tlLogicID,
        DU_TL_CONTROL, continueOnLane,
        (continueOnLane->getLength() - length), length,
        HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);
    MSNet::getInstance()->getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, newSensor);
    m_sensorMap.insert(MSLaneID_MSE2Collector(continueOnLane->getID(), newSensor));
    m_continueSensorOnLanes[lane->getID()].push_back(continueOnLane->getID());
    std::ostringstream oss;
    oss << "Continue sensor on lane " << continueOnLane->getID() << ". Current length " << (length + usedLength);
    WRITE_MESSAGE(oss.str())
    //Continue other line if needed.
    if( length + usedLength < sensorLength * 0.9)
    {
      for(std::vector<MSLane::IncomingLaneInfo>::const_iterator it = continueOnLane->getIncomingLanes().begin(); it != continueOnLane->getIncomingLanes().end(); ++it)
      {
        const MSEdge* edge = &it->lane->getEdge();
        if(!edge->isInternal() && !edge->isWalkingArea() && !edge->isCrossing())
          buildContinueSensior(lane, nb, sensorLength, it->lane, length + usedLength);
      }
    }
  }
}

void MSSOTLE2Sensors::buildSensorForOutLane(MSLane* lane,
		NLDetectorBuilder &nb){
	buildSensorForOutLane(lane,nb,OUTPUT_SENSOR_LENGTH);
}

void MSSOTLE2Sensors::buildSensorForOutLane(MSLane* lane,
		NLDetectorBuilder &nb, double sensorLength) {
	float sensorPos;
	double lensorLength;
	MSE2Collector *newSensor = NULL;
	//Check not to have more than a sensor for lane
	if (m_sensorMap.find(lane->getID()) == m_sensorMap.end()) {

		//Original:
		//double sensorLength = OUTPUT_SENSOR_LENGTH;
		//Check and set zero if the lane is not long enough for the specified sensor start
		sensorPos = (lane->getLength() - sensorLength)
				- (SENSOR_START <= lane->getLength() ? SENSOR_START : 0);

		//Check and trim if the lane is not long enough for the specified sensor lenght
		lensorLength =
				sensorLength <= (lane->getLength() - sensorPos) ?
						sensorLength : (lane->getLength() - sensorPos);

		//TODO check this lengths
		DBG(
		std::ostringstream phero_str;
		phero_str << " lane " << lane->getID() << " sensorPos= " << sensorPos
				<< " ,SENSOR_START  " << SENSOR_START << "; lane->getLength = "
				<< lane->getLength() << " ,lensorLength= " << lensorLength
				<< " ,SENSOR_LENGTH= " << INPUT_SENSOR_LENGTH;
		WRITE_MESSAGE(
				"MSSOTLE2Sensors::buildSensorForLane::" + phero_str.str());
		)

		//Create sensor for lane and insert it into the map<MSLane*, MSE2Collector*>
		newSensor = nb.buildSingleLaneE2Det(
				"SOTL_E2_lane:" + lane->getID() + "_tl:" + tlLogicID,
				DU_TL_CONTROL, lane,
				(lane->getLength() - sensorPos - lensorLength), lensorLength,
				HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);
		//newSensor = nb.buildSingleLaneE2Det("SOTL_E2_lane:"+lane->getID()+"_tl:"+tlLogicID, DU_TL_CONTROL, lane, (lane->getLength() - sensorPos- 5), lensorLength, HALTING_TIME_THRS, HALTING_SPEED_THRS, DIST_THRS);

		MSNet::getInstance()->getDetectorControl().add(
				SUMO_TAG_LANE_AREA_DETECTOR, newSensor);

		m_sensorMap.insert(MSLaneID_MSE2Collector(lane->getID(), newSensor));
		m_maxSpeedMap.insert(MSLaneID_MaxSpeed(lane->getID(), lane->getSpeedLimit()));
	}
}

unsigned int MSSOTLE2Sensors::countVehicles(MSLane* lane)
{
  return countVehicles(lane->getID());
}

/*
 * Estimate queue lenght according to the distance of the last vehicles
 */

double MSSOTLE2Sensors::getEstimateQueueLenght (std::string laneId)
{
	MSLaneID_MSE2CollectorMap::const_iterator sensorsIterator = m_sensorMap.find(laneId);
	if (sensorsIterator == m_sensorMap.end())
	{
		assert(0);
		return 0;
	}
	else
	{
		double estQL = sensorsIterator->second->getEstimateQueueLength();
		if(estQL==-1)
			return 0;
		else
			DBG(
				std::ostringstream str;
				str << "MSSOTLE2Sensors::getEstimateQueueLenght lane " << sensorsIterator->second->getLane()->getID()
						<< " laneLenght " << sensorsIterator->second->getLane()->getLength() << " estimateQueueLenght " <<  estQL;
				WRITE_MESSAGE(str.str());
			)
			return estQL;
	}
}

/*
 * Estimate queue lenght according to the distance of the last vehicles that exceed a threshold
 */

unsigned int MSSOTLE2Sensors::estimateVehicles (std::string laneId)
{
	MSLaneID_MSE2CollectorMap::const_iterator sensorsIterator = m_sensorMap.find(laneId);
	if (sensorsIterator == m_sensorMap.end())
	{
		assert(0);
		return 0;
	}
	unsigned int additional = 0;
    if(m_continueSensorOnLanes.find(laneId) != m_continueSensorOnLanes.end())
      for(std::vector<std::string>::iterator it = m_continueSensorOnLanes[laneId].begin(); it != m_continueSensorOnLanes[laneId].end(); ++ it)
      {
        if(m_sensorMap.find(*it) != m_sensorMap.end())
          additional += m_sensorMap[*it]->getEstimatedCurrentVehicleNumber(speedThresholdParam);
      }
    return sensorsIterator->second->getEstimatedCurrentVehicleNumber(speedThresholdParam) + additional;
}

unsigned int MSSOTLE2Sensors::countVehicles(std::string laneId)
{
	MSLaneID_MSE2CollectorMap::const_iterator sensorsIterator = m_sensorMap.find(laneId);
	if (sensorsIterator == m_sensorMap.end())
	{
      assert(0);
      return 0;
	}
	unsigned int additional = 0;
    if(m_continueSensorOnLanes.find(laneId) != m_continueSensorOnLanes.end())
    {
      for(std::vector<std::string>::iterator it = m_continueSensorOnLanes[laneId].begin(); it != m_continueSensorOnLanes[laneId].end(); ++ it)
      {
        unsigned int tmp = 0;
        if(getVelueFromSensor(*it, &MSE2Collector::getCurrentVehicleNumber, tmp))
          additional += tmp;
      }
    }
	return sensorsIterator->second->getCurrentVehicleNumber() + additional;
}

double MSSOTLE2Sensors::getMaxSpeed(std::string laneId)
{
	MSLaneID_MaxSpeedMap::const_iterator sensorsIteratorIn = m_maxSpeedMap.find(laneId);
	if (sensorsIteratorIn == m_maxSpeedMap.end())
	{
        assert(0);
        WRITE_ERROR("MSSOTLE2Sensors::meanVehiclesSpeed:: No lane found " + laneId);
        return 0;
	}
    return sensorsIteratorIn->second;
}

double MSSOTLE2Sensors::meanVehiclesSpeed(MSLane* lane)
{
  return meanVehiclesSpeed(lane->getID());
}

double MSSOTLE2Sensors::meanVehiclesSpeed(std::string laneId)
{
	MSLaneID_MSE2CollectorMap::const_iterator sensorsIteratorOut = m_sensorMap.find(laneId);
	if (sensorsIteratorOut == m_sensorMap.end())
	{
      assert(0);
      WRITE_ERROR("MSSOTLE2Sensors::meanVehiclesSpeed:: No lane found " + laneId);
      return 0;
	}
	double meanSpeedAcc = 0;
	unsigned int totalCarNumer = 0;
	if(m_continueSensorOnLanes.find(laneId) != m_continueSensorOnLanes.end())
      for(std::vector<std::string>::iterator it = m_continueSensorOnLanes[laneId].begin(); it != m_continueSensorOnLanes[laneId].end(); ++ it)
      {
        unsigned int number = 0;
        double mean = -1;
        if(!getVelueFromSensor(*it, &MSE2Collector::getCurrentVehicleNumber, number))
          continue;
        totalCarNumer += number;
        getVelueFromSensor(*it, &MSE2Collector::getCurrentMeanSpeed, mean);
        meanSpeedAcc += mean * (double) number;
      }
	unsigned int number = sensorsIteratorOut->second->getCurrentVehicleNumber();
	totalCarNumer += number;
	double mean = sensorsIteratorOut->second->getCurrentMeanSpeed();
	meanSpeedAcc += mean * (double) number;
	return totalCarNumer == 0 ? -1 : meanSpeedAcc / (double) totalCarNumer;
}
