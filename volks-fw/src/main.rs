#![no_std]
#![no_main]

mod consts;

use base as _;

#[rtic::app(device = hal::pac, peripherals = true, dispatchers = [SWI0_EGU0])]
mod app {

    #[cfg(feature = "heap")]
    use {crate::consts::HEAP_SIZE, cortex_m_rt};
    use {
        hal::{
            clocks::{Clocks, ExternalOscillator, Internal, LfOscStopped},
            usbd::{UsbPeripheral, Usbd},
        },
        nrf52840_hal as hal,
        pc_communications_hal::pc_coms_hal::{
            HardwareInterface, PcInterface, PcInterfaceTrait, RxStruct,
        },
        rtt_target::{rprintln, rtt_init_print},
        systick_monotonic::*,
        usb_device::{
            bus::UsbBusAllocator,
            device::{UsbDeviceBuilder, UsbVidPid},
        },
        usbd_serial::{SerialPort, USB_CLASS_CDC},
    };

    #[monotonic(binds = SysTick, default = true)]
    type MyMono = Systick<100>; // 100 Hz / 10 ms granularity

    #[shared]
    struct Shared {}

    #[local]
    struct Local {
        pc_interface: PcInterface,
    }

    #[init]
    fn init(cx: init::Context) -> (Shared, Local, init::Monotonics) {
        rtt_init_print!();
        rprintln!("[Init]");

        // Initialise the allocator if heap is enabled.
        #[cfg(feature = "heap")]
        {
            rprintln!("Initialising heap allocation!");
            unsafe {
                base::ALLOCATOR.init(cortex_m_rt::heap_start() as usize, HEAP_SIZE);
            }
        }
        let systick = cx.core.SYST;
        let mono = Systick::new(systick, 64_000_000);

        // initialise the USB and serial port peripherals
        static mut CLOCKS: Option<Clocks<ExternalOscillator, Internal, LfOscStopped>> = None;
        static mut USB_BUS: Option<UsbBusAllocator<Usbd<UsbPeripheral<'static>>>> = None;

        let clocks = Clocks::new(cx.device.CLOCK).enable_ext_hfosc();
        let usbd = cx.device.USBD;

        unsafe {
            CLOCKS = Some(clocks);
            let clocks = CLOCKS.as_ref().unwrap();
            USB_BUS = Some(Usbd::new(UsbPeripheral::new(usbd, &clocks)));
            let usb_bus = USB_BUS.as_ref().unwrap();
            let serial_port = SerialPort::new(&usb_bus);
            let usb_dev = UsbDeviceBuilder::new(usb_bus, UsbVidPid(0x16c0, 0x27dd))
                .manufacturer("Volks EEG")
                .product("Serial port")
                .serial_number("TEST")
                .device_class(USB_CLASS_CDC)
                .max_packet_size_0(64) // (makes control transfers 8x faster)
                .build();

            // pass the usb device and serial port to the pc interface
            let pc_interface = PcInterface::new(HardwareInterface {
                usb_dev,
                serial_port,
            });

            (Shared {}, Local { pc_interface }, init::Monotonics(mono))
        }
    }

    #[idle(local = [pc_interface])]
    fn idle(mut _ctx: idle::Context) -> ! {
        // local copy of the pc interface
        let pc = _ctx.local.pc_interface;

        loop {
            pc.perform_backgroud_tasks();

            // simple loopback test.
            let rx: RxStruct = pc.get_available_data();

            // if there is data then transmit if back out
            if rx.bytes_available > 0 {
                pc.set_data_to_transmit(rx.data_array, rx.bytes_available);
            }
        }
    }
}
