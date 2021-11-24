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
            gpio::{Level, Output, Pin, PushPull},
            prelude::OutputPin,
            prelude::StatefulOutputPin,
        },
        nrf52840_hal as hal,
        nrf52840_hal::clocks::{Clocks, ExternalOscillator, Internal, LfOscStopped},
        nrf52840_hal::usbd::{UsbPeripheral, Usbd},
        nrf52840_pac::Peripherals,
        pc_communications_hal::pc_coms_hal::CommunicationsInterface as pc,
        rtt_target::{rprintln, rtt_init_print},
        systick_monotonic::*,
        usb_device::device::{UsbDevice, UsbDeviceBuilder, UsbVidPid},
        usbd_serial::{SerialPort, USB_CLASS_CDC},
    };

    #[monotonic(binds = SysTick, default = true)]
    type MyMono = Systick<100>; // 100 Hz / 10 ms granularity

    #[shared]
    struct Shared {}

    #[local]
    struct Local {
        heartbeat_led: Pin<Output<PushPull>>,
        //usb_dev: UsbDevice<'static, Usbd<UsbPeripheral<'static>>>,
        //serial: SerialPort<'static, Usbd<UsbPeripheral<'static>>>,
    }

    #[init]
    fn init(cx: init::Context) -> (Shared, Local, init::Monotonics) {
        rtt_init_print!();
        rprintln!("[Init]");

        let port1 = hal::gpio::p1::Parts::new(cx.device.P1);
        let heartbeat_led = port1.p1_09.into_push_pull_output(Level::Low).degrade();

        // Test the allocator if heap is enabled.
        #[cfg(feature = "heap")]
        {
            rprintln!("Testing heap allocation!");
            unsafe {
                base::ALLOCATOR.init(cortex_m_rt::heap_start() as usize, HEAP_SIZE);
            }
            extern crate alloc;
            use alloc::vec;
            let test = vec![0..4];
            rprintln!("{:?}", test);
        }

        let systick = cx.core.SYST;
        let mono = Systick::new(systick, 64_000_000);
        //tick::spawn_after(1.secs()).unwrap();
        usb_poll::spawn_after(100.millis()).unwrap();

        let pcInterface = pc::new();

        //usb_hid::usbhid::init(&cx.device);

        let clocks = Clocks::new(cx.device.CLOCK);
        let clocks = clocks.enable_ext_hfosc();

        let usb_bus = Usbd::new(UsbPeripheral::new(cx.device.USBD, &clocks));
        let mut serial = SerialPort::new(&usb_bus);

        let mut usb_dev = UsbDeviceBuilder::new(&usb_bus, UsbVidPid(0x16c0, 0x27dd))
            .manufacturer("Volks EEG")
            .product("Serial port")
            .serial_number("TEST")
            .device_class(USB_CLASS_CDC)
            .max_packet_size_0(64) // (makes control transfers 8x faster)
            .build();

        loop {
            if !usb_dev.poll(&mut [&mut serial]) {
                continue;
            }

            let mut buf = [0u8; 64];

            match serial.read(&mut buf) {
                Ok(count) if count > 0 => {
                    // Echo back in upper case
                    for c in buf[0..count].iter_mut() {
                        if 0x61 <= *c && *c <= 0x7a {
                            *c &= !0x20;
                        } else {
                            *c = pcInterface.get_data();
                        }
                    }

                    let mut write_offset = 0;
                    while write_offset < count {
                        match serial.write(&buf[write_offset..count]) {
                            Ok(len) if len > 0 => {
                                write_offset += len;
                            }
                            _ => {}
                        }
                    }
                }
                _ => {}
            }
        }

        (
            Shared {},
            Local {
                heartbeat_led,
                //usb_dev,
                //serial,
            },
            init::Monotonics(mono),
        )
    }

    #[idle]
    fn idle(_: idle::Context) -> ! {
        loop {
            cortex_m::asm::wfi();
        }
    }

    #[task(local = [heartbeat_led])]
    fn usb_poll(mut _cx: usb_poll::Context) {
        toggle_heartbeat(&mut _cx.local.heartbeat_led);

        /*if !usb_dev.poll(&mut [&mut serial]) {
            continue;
        }

        let mut buf = [0u8; 64];

        match serial.read(&mut buf) {
            Ok(count) if count > 0 => {
                // Echo back in upper case
                for c in buf[0..count].iter_mut() {
                    if 0x61 <= *c && *c <= 0x7a {
                        *c &= !0x20;
                    }
                }

                let mut write_offset = 0;
                while write_offset < count {
                    match serial.write(&buf[write_offset..count]) {
                        Ok(len) if len > 0 => {
                            write_offset += len;
                        }
                        _ => {}
                    }
                }
            }
            _ => {}
        }*/

        // spawn again after 100mS
        usb_poll::spawn_after(100.millis()).unwrap();
    }

    /*#[task(local = [heartbeat_led])]
    fn tick(mut _cx: tick::Context) {
        rprintln!("Tick");
        toggle_heartbeat(&mut _cx.local.heartbeat_led);
        tick::spawn_after(1.secs()).unwrap();
    }*/

    fn toggle_heartbeat(led: &mut Pin<Output<PushPull>>) {
        if led.is_set_low().unwrap() {
            led.set_high().ok();
        } else {
            led.set_low().ok();
        }
    }
}
