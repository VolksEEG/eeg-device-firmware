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
        rtt_target::{rprintln, rtt_init_print},
        systick_monotonic::*,
    };

    #[monotonic(binds = SysTick, default = true)]
    type MyMono = Systick<100>; // 100 Hz / 10 ms granularity

    #[shared]
    struct Shared {}

    #[local]
    struct Local {}

    #[init]
    fn init(cx: init::Context) -> (Shared, Local, init::Monotonics) {
        rtt_init_print!();
        rprintln!("[Init]");

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

        (Shared {}, Local {}, init::Monotonics(mono))
    }

    #[idle]
    fn idle(_: idle::Context) -> ! {
        loop {
            cortex_m::asm::wfi();
        }
    }

    #[task]
    fn tick(_cx: tick::Context) {
        rprintln!("Tick");
        tick::spawn_after(1.secs()).unwrap();
    }
}
