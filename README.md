# stm32f429-openimu330
## uart connect
stm32f429 pinout
![image](https://user-images.githubusercontent.com/61956056/167083758-b241d717-47bb-4cf9-ab78-484e3dd02b1e.png)
OpenIMU 330 connect to PA2 & PA3 (UART2) input
PA10 & PA9 (UART1) connect to usb output
![16039145614418](https://user-images.githubusercontent.com/61956056/167084197-777fe84b-ca2d-423b-8246-51d23a24978c.jpg)
## build
```shell
make
```
## st-flash
```shell
st-flash write build/stm32f429-openimu330.bin 0x8000000
```
## Check USB output
```shell
screen /dev/ttyUSB0 115200 8n1
```
