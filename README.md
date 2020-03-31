# arduino
Code for arduino boards interfacing with the cell and the motor

## Scope
Since the Arduino ecosystem is greatly diversified, the reference implementation of the controller is made available a an arduino sketch. Simply change the motor ports according to your device

**Note:** This is not as efficient as the Rust implementation (stm32 repository) and mostly aimed at low-scale creation. For mass production, the stm32 repo is able to use the Cortex-M's NVIC to lower greatly the power consumption by putting the device to sleep most of the time.
