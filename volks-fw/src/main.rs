#![no_std]
#![no_main]

mod consts;

use base as _;
use usb_hid as _;

#[rtic::app(device = hal::pac, peripherals = true, dispatchers = [SWI0_EGU0])]
mod app {
    //use core::pin::Pin;

    #[cfg(feature = "heap")]
    use {crate::consts::HEAP_SIZE, cortex_m_rt};
    use {
        hal::{
            gpio::{Level, Output, Pin, PushPull},
            prelude::StatefulOutputPin,
            prelude::OutputPin,
        },
        nrf52840_hal as hal,
        rtt_target::{rprintln, rtt_init_print},
        systick_monotonic::*,
    };

    #[monotonic(binds = SysTick, default = true)]
    type MyMono = Systick<100>; // 100 Hz / 10 ms granularity

    #[shared]
    struct Shared {}

    #[local]
    struct Local {
        heartbeat_led: Pin<Output<PushPull>>,
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
        tick::spawn_after(1.secs()).unwrap();

        usb_hid::usbhid::init();

        (Shared {}, Local {heartbeat_led,}, init::Monotonics(mono))
    }

    #[idle]
    fn idle(_: idle::Context) -> ! {
        loop {
            cortex_m::asm::wfi();
        }
    }

    #[task(local = [heartbeat_led])]
    fn tick(mut _cx: tick::Context) {
        rprintln!("Tick");
        toggle_heartbeat(&mut _cx.local.heartbeat_led);
        tick::spawn_after(1.secs()).unwrap();
    }

    fn toggle_heartbeat(led: &mut Pin<Output<PushPull>>) {
        if led.is_set_low().unwrap() {
            led.set_high().ok();
        } else {
            led.set_low().ok();
        }
    }
}
