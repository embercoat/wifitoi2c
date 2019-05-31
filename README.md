# wifi2I2C
Created for esp8266 to serve as a universal bridge between i2c and the rest of the world. 

## Getting started
Flash to esp8266 chip and plug into power.
By default it sets up its own network called ESP_ap which when connected should give you a "captive portal" at 
192.168.4.1. Go there and set up your own network. If you want to, you can also cycle power to the bridge which will 
then not create its own network on boot.

## Usage
With curl, send one of the sample json-files to the /i2c endpoint.
~~~~
curl http://192.168.1.90/i2c --data @read.json
~~~~

The node address is the decimal representation of the address for the device you want to communicate with.
Sequence is a list of the operations to perform. There are three operations available
 - byte
   Value in this case is a list of the decimal representation of the bytes you wish to send
 - delay
   Value is the number of milliseconds to wait until continuing down the line.
 - read
   Length is, you guessed it, the number of bytes to read. 

As it is possible to have multiple reads in a single request, the return is a list of lists where the top list contains one list for each read and each list for a read contains the bytes returned.
~~~~
{
  "address":"54",
  "sequence": [
    {"type":"byte","value":[15,16]},
    {"type":"delay","value":10},
    {"type":"read", "length": 2},
  ]
}
~~~~