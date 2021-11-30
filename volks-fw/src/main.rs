#![no_std]
#![no_main]

mod consts;

use base as _;

#[rtic::app(device = hal::pac, peripherals = true, dispatchers = [SWI0_EGU0])]
mod app {

    #[cfg(feature = "heap")]
    use {crate::consts::HEAP_SIZE, cortex_m_rt};
    use {
        nrf52840_hal as hal,
        pc_communications_hal::pc_coms_hal::{CommunicationsInterface},
        rtt_target::{rprintln, rtt_init_print},
        systick_monotonic::*,
        hal::{
            usbd::{Usbd, UsbPeripheral}, 
            clocks::{Clocks, ExternalOscillator, Internal, LfOscStopped}
        },
        usb_device::{bus::UsbBusAllocator, device::{UsbDeviceBuilder, UsbVidPid}},
        usbd_serial::{SerialPort, DefaultBufferStore, USB_CLASS_CDC},
    };

    type UsbDevice<'a> = usb_device::device::UsbDevice<'static, Usbd<UsbPeripheral<'a>>>;

    #[monotonic(binds = SysTick, default = true)]
    type MyMono = Systick<100>; // 100 Hz / 10 ms granularity

    #[shared]
    struct Shared {}

    #[local]
    struct Local {
        usb_dev: UsbDevice<'static>,
        serial_port: SerialPort<'static, Usbd<UsbPeripheral<'static>>, DefaultBufferStore, DefaultBufferStore>,
        pc_interface: CommunicationsInterface,
    }

    #[init]
    fn init(cx: init::Context) -> (Shared, Local, init::Monotonics) {
        rtt_init_print!();
        rprintln!("[Init]");
        
        // Initialise the allocator if heap is enabled.
        #[cfg(feature = "heap")] {
            rprintln!("Initialising heap allocation!");
            unsafe {
                base::ALLOCATOR.init(cortex_m_rt::heap_start() as usize, HEAP_SIZE);
            }
        }
        let systick = cx.core.SYST;
        let mono = Systick::new(systick, 64_000_000);

        let pc_interface = CommunicationsInterface::new();
        
        static mut CLOCKS: Option<Clocks<ExternalOscillator, Internal, LfOscStopped>> = None;
        static mut USB_BUS: Option<UsbBusAllocator<Usbd<UsbPeripheral<'static>>>> = None;
        
        let clocks = Clocks::new(cx.device.CLOCK).enable_ext_hfosc();
        let usbd = cx.device.USBD;

        unsafe {
            CLOCKS = Some(clocks); 
            let clocks = CLOCKS.as_ref().unwrap();
            USB_BUS = Some(Usbd::new(UsbPeripheral::new(usbd, &clocks)));
            let usb_bus = USB_BUS.as_ref().unwrap();
            let mut serial_port = SerialPort::new(&usb_bus);
            let mut usb_dev = UsbDeviceBuilder::new(usb_bus, UsbVidPid(0x16c0, 0x27dd))
                .manufacturer("Volks EEG")
                .product("Serial port")
                .serial_number("TEST")
                .device_class(USB_CLASS_CDC)
                .max_packet_size_0(64) // (makes control transfers 8x faster)
                .build();         
 
            (
                Shared {},
                Local {
                    usb_dev,
                    serial_port,
                    pc_interface,
                },
                init::Monotonics(mono),
            )   
        }
    }

    #[idle(local = [pc_interface, usb_dev, serial_port])]
    fn idle(mut _ctx: idle::Context) -> ! {

        let usb_d = &mut _ctx.local.usb_dev;
        let srl_prt = &mut _ctx.local.serial_port;
        let pc = _ctx.local.pc_interface;
        
        loop {
            
            let data = pc.get_data();

            if data.0 {
                // there is data to send, so send it
                srl_prt.write(&[data.1, 1]);
            }
            
            if usb_d.poll(&mut [*srl_prt]) {
                let mut buf = [0u8; 64];

                match srl_prt.read(&mut buf) {
                    Ok(count) if count > 0 => {
                        // Echo back in upper case
                        for c in buf[0..count].iter() {

                            match *c as char {
                                'f' => {
                                    pc.transmit_data('I' as u8);
                                    pc.transmit_data('t' as u8);
                                    pc.transmit_data(' ' as u8);
                                    pc.transmit_data('w' as u8);
                                    pc.transmit_data('o' as u8);
                                    pc.transmit_data('r' as u8);
                                    pc.transmit_data('k' as u8);
                                    pc.transmit_data('s' as u8);
                                }
                                _ => {}
                            }
                        }
                    }
                    _ => {}
                }     
            }

            //cortex_m::asm::wfi();
        }
    }
}
