# MQTT-IDW01M1
This project is testing MQTT on mbed
- device
	- NUCLEO_F401RE
	- IDW01M1(WiFi board)
- MQTT cloud
	- Node-Red(local,in my RaspberryPi)
- Toolchain
	- ARM_GCC
- Tools
	- mbed cli
	- MQTTlens

## compile
First you need dowload mbed lib
```
mbed deploy
```
Setting toolchain and target
```
mbed toolcahin $(yourtoolchain)
mbed target $(your target)
``` 
compile
```
mbed compile
```


## MQTT step
### connect to your AP
### connect to your host
### MQTT connect
### Subscribe
### Publish

## MQTT function
- Subscribe
- Publish

## MQTT variables
- QOS


## Reference material
- [X-NUCLEO-IDW01M1 Wi-Fi expansion board](https://os.mbed.com/components/X-NUCLEO-IDW01M1/)
- [IDW01M1_Cloud_IBM](https://os.mbed.com/teams/ST/code/IDW01M1_Cloud_IBM/)
