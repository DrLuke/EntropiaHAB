# EntropiaHAB

Software and Hardware repository for the High Altitude Balloon project of Entropia.

## Flashing the tracker
1. Start openOCD with the correct configuration file
`openocd -f openocd -f /usr/share/openocd/scripts/board/stm32f0discovery.cfg`
2. Flash with `make flash`. Note that `make all` will also flash. If you only want to compile, use `make src/tracker.bin`
