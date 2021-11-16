use alloc_cortex_m::CortexMHeap;
use core::alloc::Layout;

#[global_allocator]
pub static ALLOCATOR: CortexMHeap = CortexMHeap::empty();

#[alloc_error_handler]
fn oom(_layout: Layout) -> ! {
    panic!("Allocator Out of Memory!");
}
