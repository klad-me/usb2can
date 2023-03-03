# usb2can
Custom firmware for SeedStudio's USB to CAN (slcan protocol)
![usb2can assembled](https://github.com/klad-me/usb2can/blob/main/img/1.jpg?raw=true)

## Flashing
Use jumper marked as P3 to enter internal bootloader of HK32F103C8T6, then use stm32flash to burn code:
![usb2can pcb](https://github.com/klad-me/usb2can/blob/main/img/2.jpg?raw=true)

```bash
stm32flash -w usb2can.bin /dev/ttyUSB0
```

## Using
Use slcand under linux to get socket-can interface:

```bash
sudo slcand -s 3 -S 460800 -t hw /dev/ttyUSB0
sudo ip link set up can0
```

For custom baudrate use [calculator](https://www.kvaser.com/support/calculators/bit-timing-calculator/) with 24MHz clock frequency for controller type SJA1000.

200kbit example setting:

```
sudo slcand -b 027A -S 460800 -t hw /dev/ttyUSB0
sudo ip link set up can0
```


## Dumping packets

```bash
candump can0
  can0  0C00D300   [8]  43 41 4E 20 23 3F 00 00
  can0  0C02D300   [8]  00 00 00 00 00 00 00 00
  can0  0C04D300   [8]  00 00 00 00 00 00 00 00
  can0  0C06D300   [8]  00 00 00 00 00 00 00 00
  can0  0C08D300   [8]  00 00 00 00 00 00 00 00
  can0  0C0BD3BC   [3]  02 51 20
```


## Sending packets

```bash
cansend can0 12345678#1122334455667788
```
