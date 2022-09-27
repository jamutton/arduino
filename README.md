# Arduino Projects
This is simply a collection of random Arduino projects for various home
automation/private-cloud ideas.  I'm toying with sensors, events and other various
control channels that don't get caught up in all the IoT hoopla.

All projects are licensed MIT unless otherwise noted.  Libraries are omitted but should be obvious and/or documented what you need.

## 67 Mustang Project
Really any restomod car.  The goal is to take a classic vehicle and up-fit it with more modern and fuel efficient drivetrain, then interface that with some custom sensors and displays.

### CarScreens
A menu control system for custom automotive controls.  Uses a TFT Touch screen and some rotary encoders in the stock radio position in my 67 Mustang as a UI to send digital signals to other car systems.  One possible Example I'm looking into for this is to display digital-dash data from the CANBUS circuit on Edelbrock fuel injection.

### Display
Part of the new dashboard project in the 67 Mustang.  Integrating some small OLED displays into the speedo/tach for display of various running parameters and sensor inputs as well as a gear indicator (for replacing the shifter with something more streamlined).

## Device Control
In home-automation, there are still a number of component that can be signaled or controlled via IR, Bluetooth, or other protocols.  These projects are for interfacing with those devices to create a control-plane based on either physical controls or MQTT signaling.

### IRSender/decoder
I had to figure out the mark/space setup of a HDMI switch for automating changing between inputs.  This was my discovery tool and output sample controls for an IR LED.

## PCoT - Private Cloud of Things
The Private cloud of things is an effort to build a Privacy-first set of IoT devices.  All devices in a PCoT Ecosystem communicate with a local MQTT server and rely on a bridge to bring-in any third-party cloud commands or send out any data to 3p Clouds.  This started as a way to play with different sensors but, given the prolifiration of different cloud-apps and the complete lack of visibility into where your data is sent, turned into a privacy-first initiative.  All the sensor code in the following listed modules operate with this in mind and most of them simply send MQTT messages or respond to a few commands.

### MQTT Topic layout
There are 4 main topic branches in a pcot topic-taxonomy:
* Announce/keepalives
* Commands
* State-changes
* Metrics
All topics start with `pcot/` followed by the options from their respective tree.  Timestamps on all messages are considered by their arrival time at the MQTT broker because we do not trust sensors to have accurate clocks.

#### Announce/Keepalive
Announce and keepalive messages both begin with `pcot/announce` but have slightly different flavors.  Announce messages happen when a sensor joins the wifi or starts up, whereas keepalive messages (`pcot/announce/$SENSORID`) happen at specific intervals and should only be repeated if the node remains online and subscribed to a command channel.  Both share the same message format `$SENSORID:$UPTIME:$TYPEID`.  Subscirbers to the announce topic can use the sensorid and type fields to build knowledge of what sensors are reporting in (for dynamic dashboards, etc). Keepalive messages allow a system to listen for awake sensors and use the typeid to know what commands are available.  Typeid's are meant to be arbitrary for a given private cloud (you make up your own).

#### Commands
Commands are specific to a typeid.  Some examples of commands might be:
* measure-on-command
* open/close-contacts (switch a relay)
* send IR
...and more.  Commands are acted on by sensor that remain online to subscribe to command topics.  Every sensor that supports commands should subscribe to 2 different topics:
* `pcot/cmd/$SENSORID` - Allows the sensor to be directly addressed
* `pcot/cmd/$TYPEID` - Allows all sensors of the same type to be addressed
A sensor subscribed to these topics should send keepalives, as denoted above, at a minimum every 25 seconds.  This allows devices to support a 30-second SLA to obtain all available targets for a command.  Sensor commands are the message body delivered to the topics and have a standard but extensible structure.  All sensor commands use the following format: `$CMD:$RESPONSETOPIC:$ARGS` where the `$CMD` is specific to the `TYPEID`, `$RESPONSETOPIC` is any topic that the sender want's a response on and `$ARGS` is a <typically> comma-delimited list of arguments.  How the sensor interprets the args is specific to the `TYPEID` so document your types in your own system.  Any sensor that receives a command with a response topic shoule execute that command and send any formatted response to the response topic as defined by the `TYPEID` contract.

#### State Changes
State changes are a special case of pcot sensors.  They can be triggered by a sensor directly or by a system interpreting the sensor.  State-changes introduce a concept that will come up again in metrics called a `LOCATIONID`.  A location id can be very-very specific (e.g. a specific spot on a wall) or they can be more generic (e.g. a site or room).  Essentially, location id meanings are specific to the install and the possible states are specific to the `TYPEID`.  This means you can craft a set of metrics that a listener can combine to a state change and you just need to define a type to support the state-change.  State change message-topics follow the pattern of `pcot/states/$LOCATIONID` while messages sent to this topic follow the patern of `$SENSORID:$TYPEID:$STATE:$VALUE` where `STATE` and `VALUE` are defined as options of the `TYPEID`.

#### Metrics
Metrics are expected to be the majority of pcot sensors.  They measure a value at a specific interval (typically 60 seconds) and message that value to the local MQTT server.  Metrics are location-specific, meaning they target a `LOCATIONID` and are as-specific as that location id.  They can target the surface-temperature of a pump or the smoke-ppm of a region.  Metrics follow a topic-hierarchy that includes the locationid, the value-name from the TYPEID and the sensorid in the following format: `pcot/sensors/$LOCATIONID/$VALUENAME/$SENSORID` and the message contains the discrete value.  Using the discrete value instead of a compound like others allows metrics to be consumed directly by tools like graphana.

### PCoT Projects

#### gasDetect
Just playing around with a gas sensor that is supposed to pickup CO, ammonia, methane/propane/CNG and other nastys

#### tempMonitor
Another small sensor setup for measuring temperature.  Ideally, something that can be eventually integrated on the WiFi for eventing the temp in various rooms.

#### motionsensor
For my PCoT (Private Cloud of Things) service. A MQTT event generator for motion.  Uses the ethernet library and zeroconf to discover the local MQTT server, then publishes motion events and heartbeats to the MQTT server.  The idea is that so long as you're getting heartbeats you timeout events but you'd latch the event if the heartbeat goes away.  motionsensor is meant to be tied into listeners that can do things like activate video-archiving or start a webrtc session with a remote host.

#### LeakSensor
This is a perodic awake sensor for measuring a floor-mounted water-leak detector.  It's basically a contact test for voltage leakage across a set of submergable contacts.  The sketch is designed to send perodic binary messages on whether water has been detected such that the contacts are bridged.  It does not support any command channel and disconnects from wifi in-between measurements.  It's a suitable starter for a low-power battery operated device with a deep-sleep cycle but in my case it's connected to mains power.

#### PressureSender
This is a periodic awake sensor for measuring the pressure of a liquid closed system (water pipe, fermentation vessel or other pressure system).  This specific sketch works with a 0-200PSI sensor operating on 0.5-4.5v scale.  It does not support any command channel and disconnects from wifi in-between measurements.  It's a suitable starter for a low-power battery operated device with a deep-sleep cycle but in my case it's connected to mains power.

#### DallasTempSender
This is a periodic awake sensor for measuring temperature using the Dallas Onewire temperature library.  It uses very streamlined sensors that can be epoxied-in, clamped to a surface or submerged to measure the temperature of a medium or surface.  It does not support any command channel and disconnects from wifi in-between measurements.  It's a suitable starter for a low-power battery operated device with a deep-sleep cycle but in my case it's connected to mains power.
