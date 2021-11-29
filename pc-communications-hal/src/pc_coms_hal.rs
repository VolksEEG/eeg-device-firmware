use ringbuf::{RingBuffer, Producer, Consumer};

pub struct CommunicationsInterface {
    // public

    // private
    tx_producer: Producer<u8>,
    tx_consumer: Consumer<u8>,
}

impl CommunicationsInterface {
    pub fn new() -> CommunicationsInterface {
        
        let tx_buffer = RingBuffer::<u8>::new(64);
        let (tx_producer, tx_consumer) = tx_buffer.split();

        CommunicationsInterface {
            tx_producer,
            tx_consumer,
        }
    }

    pub fn get_data(&mut self) -> (bool, u8) {
        // check if there is any data to get
        if self.tx_consumer.is_empty() {
            return (false, 0);
        }

        return (true, self.tx_consumer.pop().unwrap());
    }

    pub fn transmit_data(&mut self, data: u8) -> bool {
        // can we add to the transmit buffer
        if self.tx_producer.is_full() {
            return false;
        }

        self.tx_producer.push(data);

        return true;
    }
}
