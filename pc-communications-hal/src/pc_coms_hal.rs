use nrf52840_hal::{
    clocks::{ExternalOscillator, Internal, LfOscStopped},
    pac::{CLOCK, USBD},
    usbd::{UsbPeripheral, Usbd},
    Clocks,
};
use usb_device::{
    class_prelude::UsbBusAllocator,
    device::{UsbDevice, UsbDeviceBuilder, UsbVidPid},
};
use usbd_serial::{DefaultBufferStore, SerialPort, USB_CLASS_CDC};

pub struct CommunicationsInterface {
    //<'coms_interface_life> {
    // public

    // private
    txBuf: [u8; 64],
    txIpIndex: usize,
    txOpIndex: usize,
    txCount: usize,
    /*clock: Clocks<ExternalOscillator, Internal, LfOscStopped>,
    usb_bus: UsbBusAllocator<Usbd<UsbPeripheral<'coms_interface_life>>>,
    serial_port: SerialPort<
        'coms_interface_life,
        Usbd<UsbPeripheral<'coms_interface_life>>,
        DefaultBufferStore,
        DefaultBufferStore,
    >,
    usb_device: UsbDevice<'coms_interface_life, Usbd<UsbPeripheral<'coms_interface_life>>>,*/
}

impl CommunicationsInterface {
    //<'static> {
    pub fn new(clk: CLOCK, usb_dev: USBD) -> CommunicationsInterface {
        //<'static> {
        /*let clock = Clocks::new(clk);
                let clock = clock.enable_ext_hfosc();

                let usb_bus = Usbd::new(UsbPeripheral::new(usb_dev, &clock));
                let mut serial_port = SerialPort::new(&usb_bus);
                let mut usb_device = UsbDeviceBuilder::new(&usb_bus, UsbVidPid(0x16c0, 0x27dd))
                    .manufacturer("Volks EEG")
                    .product("Serial port")
                    .serial_number("TEST")
                    .device_class(USB_CLASS_CDC)
                    .max_packet_size_0(64) // (makes control transfers 8x faster)
                    .build();
        */

        let txBuf: [u8; 64] = [0; 64];

        let txIpIndex = 0;
        let txOpIndex = 0;
        let txCount = 0;

        CommunicationsInterface {
            txBuf,
            txIpIndex,
            txOpIndex,
            txCount,
            /*clock,
            usb_bus,
            serial_port,
            usb_device,*/
        }
    }

    pub fn get_data(&mut self) -> (bool, u8) {
        // check if there is any data to get
        if (0 == self.txCount) {
            return (false, 0);
        }

        let result = (true, self.txBuf[self.txOpIndex]);

        // advance the output index and wrap around
        self.txOpIndex = if 64 == (self.txOpIndex + 1) {
            0
        } else {
            self.txOpIndex + 1
        };

        return result;
    }
}

/*pub trait BidirectionalComsInterface {
    fn new();
    fn get_available_data(&self) -> u8;
    fn transmit_data(data: u8);
}

pub struct ComsInterface {}

impl BidirectionalComsInterface for ComsInterface {
    fn new() {}

    fn get_available_data(&self) -> u8 {
        0
    }

    fn transmit_data(data: u8) {}
}*/
