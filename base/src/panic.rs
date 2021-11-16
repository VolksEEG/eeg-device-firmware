#[inline(never)]
#[panic_handler]
#[cfg(not(feature = "panic"))]
fn panic(info: &PanicInfo) -> ! {
    cortex_m::interrupt::disable();
    loop {
        compiler_fence(Ordering::SeqCst);
    }
}

#[cfg(feature = "panic")]
pub use panic_probe as _;
