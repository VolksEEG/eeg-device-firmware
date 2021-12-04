//! Abstraction layer around the PC Communications interface

use nrf52840_hal::usbd::{UsbPeripheral, Usbd};
use usb_device::device::UsbDevice;
use usbd_serial::{DefaultBufferStore, SerialPort};

/// Result of the background task function
/// this may not be necessary
pub enum BackgroundTaskResult {
    Complete,
    MoreToDo,
}

/// Structure returned from the get_available_data function
pub struct RxStruct {
    pub bytes_available: u8,
    pub data_array: [u8; 64],
}

/// Definition of the hardware interface to be used by this module
pub struct HardwareInterface {
    pub usb_dev: UsbDevice<'static, Usbd<UsbPeripheral<'static>>>,
    pub serial_port:
        SerialPort<'static, Usbd<UsbPeripheral<'static>>, DefaultBufferStore, DefaultBufferStore>,
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
    tx_buffer: [u8; 128],
    tx_ip: u8,
    tx_op: u8,
    tx_count: u8,
    rx_buffer: [u8; 128],
    rx_ip: u8,
    rx_op: u8,
    rx_count: u8,
}

/// Private implementations for PcInterface
impl PcInterface {
    fn add_byte_to_rx_buffer(&mut self, data: u8) -> bool {
        if self.rx_count >= 128 {
            return false;
        }

        // add the byte to the buffer
        self.rx_buffer[self.rx_ip as usize] = data;

        // increment and wrap the index
        self.rx_ip = if self.rx_ip + 1 == 128 {
            0
        } else {
            self.rx_ip + 1
        };

        // increment the count
        self.rx_count += 1;

        return true;
    }

    fn get_byte_from_rx_buffer(&mut self) -> (bool, u8) {
        if 0 == self.rx_count {
            return (false, 0);
        }

        // get the next byte in the buffer
        let result = self.rx_buffer[self.rx_op as usize];

        // increment and wrap the index
        self.rx_op = if self.rx_op + 1 == 128 {
            0
        } else {
            self.rx_op + 1
        };

        // decrement the count
        self.rx_count -= 1;

        // return the data
        return (true, result);
    }

    fn add_byte_to_tx_buffer(&mut self, data: u8) -> bool {
        if self.tx_count >= 128 {
            return false;
        }

        // add the byte to the buffer
        self.tx_buffer[self.tx_ip as usize] = data;

        // increment and wrap the index
        self.tx_ip = if self.tx_ip + 1 == 128 {
            0
        } else {
            self.tx_ip + 1
        };

        // increment the count
        self.tx_count += 1;

        return true;
    }

    fn get_byte_from_tx_buffer(&mut self) -> (bool, u8) {
        if 0 == self.tx_count {
            return (false, 0);
        }

        // get the next byte in the buffer
        let result = self.tx_buffer[self.tx_op as usize];

        // increment and wrap the index
        self.tx_op = if self.tx_op + 1 == 128 {
            0
        } else {
            self.tx_op + 1
        };

        // decrement the count
        self.tx_count -= 1;

        // return the data
        return (true, result);
    }
}

/// Implementing the PcInterfaceTrait for the PcInterface structure
impl PcInterfaceTrait for PcInterface {
    /// new:  creates a new insctance of PcInterface using the passed Hardware Interface
    fn new(hw_interface: HardwareInterface) -> PcInterface {
        // create the struct to return.
        PcInterface {
            hw_interface,
            tx_buffer: [0u8; 128],
            tx_ip: 0,
            tx_op: 0,
            tx_count: 0,
            rx_buffer: [0u8; 128],
            rx_ip: 0,
            rx_op: 0,
            rx_count: 0,
        }
    }

    /// perform_backgroud_tasks:
    /// Performs background tasks for the USB interface.
    fn perform_backgroud_tasks(&mut self) -> BackgroundTaskResult {
        // temporary rx buffer.
        let mut buf = [0u8; 64];
        let mut count = 0;

        let tx_data = self.get_byte_from_tx_buffer();

        let tx: bool;
        let rx: bool;

        // scoping brackets to split the mutable self instances required for
        // the usb_d and srl_prt variables from the instances required by
        // GetByteFromTxBuffer and AddByteToRxBuffer.
        {
            // local references to the hardware interface.
            let usb_d = &mut self.hw_interface.usb_dev;
            let srl_prt = &mut self.hw_interface.serial_port;

            // Poll the USB device for any received data
            rx = if usb_d.poll(&mut [srl_prt]) {
                // read the data
                match srl_prt.read(&mut buf) {
                    Ok(cnt) if cnt > 0 => {
                        count = cnt;
                        true
                    }
                    _ => false,
                }
            } else {
                // no data recieved so set rx to false
                false
            };

            tx = if tx_data.0 {
                // write the data to the serial port
                srl_prt.write(&[tx_data.1, 1]);

                // data has been sent so set tx to true
                true
            } else {
                // no data to transmit so set tx to false
                false
            };
        }

        // copy all the received bytes into the rx buffer.
        for elem in buf[0..count].iter() {
            self.add_byte_to_rx_buffer(*elem);
        }

        // if data has been received or transmitted then there may be more data to recieve
        // or transmit, so return that there is more to do, otherwise say we are complete.
        return if tx || rx {
            BackgroundTaskResult::MoreToDo
        } else {
            BackgroundTaskResult::Complete
        };
    }

    /// get_available_data:
    /// returns a struct containing up to 64 bytes of available data.
    fn get_available_data(&mut self) -> RxStruct {
        if 0 == self.rx_count {
            // no data received so return an empty array and a count of 0
            return RxStruct {
                bytes_available: 0,
                data_array: [0u8; 64],
            };
        }

        let mut result = [0u8; 64];
        let mut count: u8 = 0;

        let mut rx_data = self.get_byte_from_rx_buffer();

        // pop off up to 64 bytes of received data.
        while count < 64 && rx_data.0 {
            result[count as usize] = rx_data.1;
            count += 1;
            rx_data = self.get_byte_from_rx_buffer();
        }

        // and return the data
        return RxStruct {
            bytes_available: count,
            data_array: result,
        };
    }

    /// set_data_to_transmit:
    /// add the passed data to the transmit buffer, to be sent out
    /// as part of the background task
    fn set_data_to_transmit(&mut self, data: [u8; 64], count: u8) {
        // push all the data to the tx buffer
        for b in data[0..count as usize].iter() {
            self.add_byte_to_tx_buffer(*b);
        }
    }
}
