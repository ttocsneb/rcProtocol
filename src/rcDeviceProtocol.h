/*
  rcDeviceProtocol.h - Library for Device for RCProtocol.
  Created by Benjamin Jacobs, October 2, 2017
*/

#ifndef __RCDEVICEPROTOCOL_H__
#define __RCDEVICEPROTOCOL_H__

#include <RF24.h>

#include "rcSettings.h"
#include "rcGlobal.h"

#ifndef __RF24_H__
#error "rcDeviceProtocol Requires the tmrh20 RF24 Library: https://github.com/nRF24/RF24"
#endif

//Userdefined Constants
//Global constants can be found in rcGlobal.h

//Error Constants
//Global constatns can be found in rcGlobal.h

/**
 * Communication Protocol for receivers
 */
class DeviceProtocol : RCGlobal {
public:
  /**
   * Save the transmitter id to non-volitile memory.
   *
   * Very simple, all that needs to be done, is to save the Id to memory,
   * so that when the receiver tries to connect, it will know which id to
   * request to connect with.
   *
   * @param id 5 byte char array
   */
  typedef void (saveRemoteID)(const uint8_t* id);
  /**
   * Load the transmitter id from non-volitile memory.
   *
   * Load the transmitter id set from saveRemoteID() into id
   *
   * @param id 5 byte array to put remote id in.
   */
  typedef void (loadRemoteID)(uint8_t* id);
  /**
   * Check if the device was previously connected to the remote.
   *
   * This checks the data set from setConnected(), and should be non-volitile
   *
   * @return true if connected.
   */
  typedef bool (checkConnected)();
  /**
   * Set whether the device is connected to the remote.
   *
   * @note This should be non-volitile, meaning the value set by this should
   * remain after a power cycle or reset
   *
   * @param connected true if connected, false if not.
   */
  typedef void (setConnected)(bool connected);

  /**
   * Constructor
   *
   * Creates a new instance of the protocol. You create an instance and send a reference
   * to the RF24 driver as well as the id of the remote
   *
   * @param tranceiver A reference to the RF24 chip, this allows you to create your own instance,
   * allowing multi-platform support
   * @param deviceId The 5 byte char array of the receiver's ID: ex "MyRcr"
   */
  DeviceProtocol(RF24* tranceiver, const uint8_t deviceId[]);

  /**
   * Begin the Protocol
   *
   * If the system power cycled or reset while in use, it will try to reconnect
   * immidiately.
   *
   * @note There is no need to begin the RF24 driver, as this function already
   * does this for you
   *
   * @param settings RCSettings
   * @param checkConnected checkConnected()
   * @param loadRemoteID loadRemoteID()
   *
   * @return 0
   * @return 1 if reconnected to remote
   * @return -1 if unable to reconnect to remtoe
   */
  int8_t begin(RCSettings* settings, checkConnected checkConnected,
               loadRemoteID loadRemoteID);

  /**
   * Attempt to pair with a transmitter
   *
   * @note The transmitter you are trying to pair with should also be in pair mode
   *
   * @param saveRemoteID A function pointer to save the id of the transmitter.
   *
   * @return 0 if successful
   * @return #RC_ERROR_ALREADY_CONNECTED if already connected to remote
   * @return #RC_ERROR_TIMEOUT if no transmitter was found.
   * @return #RC_ERROR_LOST_CONNECTION if transmitter stopped replying
   */
  int8_t pair(saveRemoteID saveRemoteID);

  /**
   * Check if the receiver is connected with a transmitter.
   *
   * @return true when connected.
   */
  bool isConnected();

  /**
   * Attempt to pair with a transmitter
   *
   * @note The transmitter you are trying to connect with should also be in connect mode,
   * as well as paired with this device
   *
   * @param loadRemoteID loadRemoteID()
   * @param setConnected setConnected()
   *
   * @return 0 if successful
   * @return #RC_ERROR_ALREADY_CONNECTED if already connected to remote
   * @return #RC_ERROR_TIMEOUT if no transmitter was found
   * @return #RC_ERROR_LOST_CONNECTION if transmitter stopped replying
   * @return #RC_ERROR_CONNECTION_REFUSED if the transmitter refused to connect
   * @return #RC_ERROR_BAD_DATA if the settings are not properly set, or
   * the transmitter sent unexpected data
   */
  int8_t connect(loadRemoteID loadRemoteID, setConnected setConnected);

  /**
   * Update the communications with the currently connected device
   *
   * If there was a packet sent, it will process it.
   *
   * @param channels RCSettings.setNumChannels() size array that is set
   * when a standard packet is received.
   * @param telemetry RCSettings.setPayloadSize() size array of telemetry
   * data to send to the transmitter
   * @param setConnected setConnected()
   *
   * @return 1 if channels were updated
   * @return 0 if nothing happened
   * @return #RC_ERROR_NOT_CONNECTED if not connected
   */
  int8_t update(uint16_t channels[], uint8_t telemetry[],
                setConnected setConnected);

  /**
   * Get pointer for the current settings
   *
   * @return settings
   */
  RCSettings* getSettings();
private:

  const uint8_t* _deviceId;
  uint8_t _remoteId[5];
  bool _isConnected;


  /**
   * Check if a packet is available, and read it to returnData
   *
   * @param returnData data to set if data was received
   * @param dataSize size of returnData in bytes
   * @param telemetry Data to Send back.  Note: telemetry won't be sent if
   * ack payloads are disabled
   * @param telemetrySize size of telemetry in bytes
   *
   * @return 1 if data is available
   * @return 0 if nothing is available
   * @return #RC_ERROR_NOT_CONNECTED if not connected
   */
  int8_t check_packet(void* returnData, uint8_t dataSize, void* telemetry,
                      uint8_t telemetrySize);
  int8_t check_packet(void* returnData, uint8_t dataSize);

};

#endif
