use nrf52840_hal::{
    clocks::{ExternalOscillator, Internal, LfOscStopped},
    pac::{CLOCK, USBD},
    usbd::{UsbPeripheral, Usbd},
    Clocks,
};
use ringbuf::{Consumer, Producer, RingBuffer};
use usb_device::{
    class_prelude::UsbBusAllocator,
    device::{UsbDevice, UsbDeviceBuilder, UsbVidPid},
};
use usbd_serial::{DefaultBufferStore, SerialPort, USB_CLASS_CDC};

pub struct CommunicationsInterface {
    //<'coms_interface_life> {
    // public

    // private
    tx_producer: Producer<u8>,
    tx_consumer: Consumer<u8>,
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
    pub fn new() -> CommunicationsInterface { //clk: CLOCK, usb_dev: USBD) -> CommunicationsInterface {
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

        let txBuffer = RingBuffer::<u8>::new(64);
        let (mut tx_producer, mut tx_consumer) = txBuffer.split();

        CommunicationsInterface {
            tx_producer,
            tx_consumer,
            /*clock,
            usb_bus,
            serial_port,
            usb_device,*/
        }
    }

    pub fn get_data(&mut self) -> (bool, u8) {
        // check if there is any data to get
        if (self.tx_consumer.is_empty()) {
            return (false, 0);
        }

        return (true, self.tx_consumer.pop().unwrap());
    }

    pub fn transmit_data(&mut self, data: u8) -> bool {
        // can we add to the transmit buffer
        if (self.tx_producer.is_full()) {
            return false;
        }

        self.tx_producer.push(data);

        return true;
    }
}
