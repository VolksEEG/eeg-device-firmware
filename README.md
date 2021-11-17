# eeg-device-firmware
Firmware repository for the VolksEEG data acquisition device.

To build the firmware:
```
cargo build
```

To run the firmware with a heap:
```
cargo +nightly run --features heap
```

To view the size of the demo firmware:
```
cargo size --bin demo --release
```