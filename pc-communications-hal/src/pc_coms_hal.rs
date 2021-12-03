//! Abstraction layer around the PC Communications interface

use nrf52840_hal::usbd::{UsbPeripheral, Usbd};
use ringbuf::{RingBuffer, Producer, Consumer};
use usb_device::device::UsbDevice;
use usbd_serial::{DefaultBufferStore, SerialPort};

/// Result of the background task function 
/// this may not be necessary
pub enum BackgroundTaskResult{
    Complete,
    MoreToDo,
}

/// Structure returned from the get_available_data function
pub struct RxStruct {
    pub BytesAvailable: u8,
    pub DataArray: [u8; 64]
}

/// Definition of the hardware interface to be used by this module
pub struct HardwareInterface {
    pub usb_dev: UsbDevice<'static, Usbd<UsbPeripheral<'static>>>,
    pub serial_port: SerialPort<'static, Usbd<UsbPeripheral<'static>>, DefaultBufferStore, DefaultBufferStore>,    
}

/// PcInterfaceTrait which defines the interface to the modlue.
pub trait PcInterfaceTrait {
    fn new(hardware_interface: HardwareInterface) -> PcInterface;
    fn perform_backgroud_tasks(&mut self) -> BackgroundTaskResult;
    fn get_available_data(&mut self) -> RxStruct;
    fn set_data_to_transmit(&mut self, data: [u8; 64], count: u8);
}

/// PcInterface structure which holds all the data for the module
pub struct PcInterface {
    // public

    // private
    hw_interface: HardwareInterface,
    tx_producer: Producer<u8>,
    tx_consumer: Consumer<u8>,
    
    rx_producer: Producer<u8>,
    rx_consumer: Consumer<u8>,
}

/// Implementing the PcInterfaceTrait for the PcInterface structure
impl PcInterfaceTrait for PcInterface {

    /// new:  creates a new insctance of PcInterface using the passed Hardware Interface
    fn new(hw_interface: HardwareInterface) -> PcInterface {
        
        // set up the transmit buffers
        let tx_buffer = RingBuffer::<u8>::new(128);
        let (tx_producer, tx_consumer) = tx_buffer.split();

        // and the receive buffers
        let rx_buffer = RingBuffer::<u8>::new(128);
        let (rx_producer, rx_consumer) = rx_buffer.split();

        // create the struct to return.
        PcInterface {
            hw_interface,
            tx_producer,
            tx_consumer,
            rx_producer,
            rx_consumer,
        }
    }

    /// perform_backgroud_tasks: 
    /// Performs background tasks for the USB interface. 
    fn perform_backgroud_tasks(&mut self) -> BackgroundTaskResult {

        // local references to the hardware interface.
        let usb_d = &mut self.hw_interface.usb_dev;
        let srl_prt = &mut self.hw_interface.serial_port;

        // Poll the USB device for any received data 
        let rx = if usb_d.poll(&mut [srl_prt]) {
            let mut buf = [0u8; 64];

            // read the data
            match srl_prt.read(&mut buf) {
                Ok(count) if count > 0 => {
                    // Add all bytes to the rx_producer
                    for elem in buf[0..count].iter() {
                        self.rx_producer.push(*elem);
                    }
                }
                _ => {}
            }
            // data has been recieved so set rx to true
            true
        } else {
            // no data recieved so set rx to false
            false
        };

        let tx = if self.tx_consumer.is_empty() {
            // no data to transmit so set tx to false
            false
        } else {

            // get the next byte to send
            let value = self.tx_consumer.pop();

            // write it to the serial port
            if value.is_some() {
                srl_prt.write(&[value.unwrap(), 1]);
            }

            // data has been sent so set tx to true
            true
        };

        // if data has been received or transmitted then there may be more data to recieve 
        // or transmit, so return that there is more to do, otherwise say we are complete.
        return if tx || rx { BackgroundTaskResult::MoreToDo } else { BackgroundTaskResult::Complete };
    }

    /// get_available_data: 
    /// returns a struct containing up to 64 bytes of available data.
    fn get_available_data(&mut self) -> RxStruct {

        if self.rx_consumer.is_empty() {
            // no data received so return an empty array and a count of 0
            return RxStruct{BytesAvailable: 0, DataArray: [0u8; 64]};
        }

        let mut result = [0u8; 64];
        let mut count: u8 = 0;

        // pop off up to 64 bytes of received data.
        while count < 64 && !self.rx_consumer.is_empty() {
            result[count as usize] = self.rx_consumer.pop().unwrap();
            count += 1;
        }

        // and return the data
        return RxStruct{BytesAvailable: count, DataArray: result};
    }

    /// set_data_to_transmit:
    /// add the passed data to the transmit buffer, to be sent out 
    /// as part of the background task
    fn set_data_to_transmit(&mut self, data: [u8; 64], count: u8) {

        // push all the data to the tx buffer
        for b in data[0..count as usize].iter() {
            self.tx_producer.push(*b);
        } 
    }
}
