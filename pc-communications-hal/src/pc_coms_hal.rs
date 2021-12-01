
use nrf52840_hal::usbd::{UsbPeripheral, Usbd};
use ringbuf::{RingBuffer, Producer, Consumer};
use usb_device::device::UsbDevice;
use usbd_serial::{DefaultBufferStore, SerialPort};

pub enum BackgroundTaskResult{
    Complete,
    MoreToDo,
}

pub struct RxStruct {
    pub BytesAvailable: u8,
    pub DataArray: [u8; 64]
}

pub struct HardwareInterface {
    pub usb_dev: UsbDevice<'static, Usbd<UsbPeripheral<'static>>>,
    pub serial_port: SerialPort<'static, Usbd<UsbPeripheral<'static>>, DefaultBufferStore, DefaultBufferStore>,    
}

pub trait PcInterfaceTrait {
    fn new(hardware_interface: HardwareInterface) -> PcInterface;
    fn perform_backgroud_tasks(&mut self) -> BackgroundTaskResult;
    fn get_available_data(&mut self) -> RxStruct;
    fn set_data_to_transmit(&mut self, data: [u8; 64], count: u8);
}

pub struct PcInterface {
    // public

    // private
    hw_interface: HardwareInterface,
    tx_producer: Producer<u8>,
    tx_consumer: Consumer<u8>,
    
    rx_producer: Producer<u8>,
    rx_consumer: Consumer<u8>,
}

impl PcInterfaceTrait for PcInterface {
    fn new(hw_interface: HardwareInterface) -> PcInterface {
        
        let tx_buffer = RingBuffer::<u8>::new(128);
        let (tx_producer, tx_consumer) = tx_buffer.split();

        let rx_buffer = RingBuffer::<u8>::new(128);
        let (rx_producer, rx_consumer) = rx_buffer.split();

        PcInterface {
            hw_interface,
            tx_producer,
            tx_consumer,
            rx_producer,
            rx_consumer,
        }
    }

    fn perform_backgroud_tasks(&mut self) -> BackgroundTaskResult {

        let usb_d = &mut self.hw_interface.usb_dev;
        let srl_prt = &mut self.hw_interface.serial_port;

        let rx = if usb_d.poll(&mut [srl_prt]) {
            let mut buf = [0u8; 64];

            match srl_prt.read(&mut buf) {
                Ok(count) if count > 0 => {
                    // Add all bytes to the rx_producer
                    for elem in buf[0..count].iter() {
                        self.rx_producer.push(*elem);
                    }
                }
                _ => {}
            }

            true
        } else {
            false
        };

        let tx = if self.tx_consumer.is_empty() {
            false
        } else {

            let value = self.tx_consumer.pop();

            if value.is_some() {
                srl_prt.write(&[value.unwrap(), 1]);
            }

            true
        };

        return if tx || rx { BackgroundTaskResult::MoreToDo } else { BackgroundTaskResult::Complete };
    }

    fn get_available_data(&mut self) -> RxStruct {

        if self.rx_consumer.is_empty() {
            return RxStruct{BytesAvailable: 0, DataArray: [0u8; 64]};
        }

        let mut result = [0u8; 64];
        let mut count: u8 = 0;

        while count < 64 && !self.rx_consumer.is_empty() {
            result[count as usize] = self.rx_consumer.pop().unwrap();
            count += 1;
        }

        return RxStruct{BytesAvailable: count, DataArray: result};
        
    }

    fn set_data_to_transmit(&mut self, data: [u8; 64], count: u8) {

        for b in data[0..count as usize].iter() {
            self.tx_producer.push(*b);
        } 
    }
}
