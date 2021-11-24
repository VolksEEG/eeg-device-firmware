pub struct CommunicationsInterface {
    data: u8,
}

impl CommunicationsInterface {
    pub fn new() -> CommunicationsInterface {
        CommunicationsInterface { data: 0x31 }
    }

    pub fn get_data(&self) -> u8 {
        return self.data;
    }
}

/*pub trait BidirectionalComsInterface {
    fn new();
    fn get_available_data(&self) -> u8;
    fn transmit_data(data: u8);
}

pub struct ComsInterface {}

impl BidirectionalComsInterface for ComsInterface {
    fn new() {}

    fn get_available_data(&self) -> u8 {
        0
    }

    fn transmit_data(data: u8) {}
}*/
