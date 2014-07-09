BOARD=arduino:avr:uno
PORT=/dev/ttyUSB1

upload: build
	arduino --upload --board $(BOARD) --port $(PORT) --verbose-build FablightFirmware.ino --pref build.path="$(shell pwd)/build"

build:
	mkdir build

.PHONY: upload
