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
        pc_communications_hal::pc_coms_hal::CommunicationsInterface as pc,
        rtt_target::{rprintln, rtt_init_print},
        systick_monotonic::*,
    };

    #[monotonic(binds = SysTick, default = true)]
    type MyMono = Systick<100>; // 100 Hz / 10 ms granularity

    #[shared]
    struct Shared {}

    #[local]
    struct Local {
        pc_interface: pc,
    }

    #[init]
    fn init(cx: init::Context) -> (Shared, Local, init::Monotonics) {
        rtt_init_print!();
        rprintln!("[Init]");

        let systick = cx.core.SYST;
        let mono = Systick::new(systick, 64_000_000);

        pc_coms_task::spawn_after(100.millis()).unwrap();

        let pc_interface = pc::new();

        (
            Shared {},
            Local {
                pc_interface,
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

    #[task(local = [pc_interface])]
    fn pc_coms_task(mut _cx: pc_coms_task::Context) {

        let tx = false; //_cx.local.pc_interface.transmit_data();
        let rx = _cx.local.pc_interface.get_data();

        if rx.0 {
            // data received
            // do something with it here 

        }

        match tx || rx.0 {
            true => { 
                // data has been received or transmitted to spawn 
                // again incase there is more to receive or transmit
                pc_coms_task::spawn().unwrap(); 
            }
            false => { 
                // nothing transmitted or received, so wait a bit
                pc_coms_task::spawn_after(100.millis()).unwrap(); 
            }
        };
    }
}
