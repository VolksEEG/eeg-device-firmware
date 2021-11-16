#![no_std]
#![cfg_attr(feature = "heap", feature(alloc_error_handler))]
#[cfg(feature = "heap")]
mod allocator;
#[cfg(feature = "heap")]
pub use allocator::ALLOCATOR;
mod panic;
