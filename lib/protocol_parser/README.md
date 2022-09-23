# Protocol Parser
## Overview
This library deals with parsing and generation of communications packets between this firmware and a PC to the requirements of the protocol defined at https://github.com/VolksEEG/VolksEEG/wiki/FrontEndToPcApi.

## Architecture 
```mermaid
classDiagram
    class IProtocolTransmission{
       <<interface>>
        +SendPayloadToPc()
        +UpdateIdToAcknowledge()
        +UpdateAcknowledgedId()
    }

    class IPcCommunications{
        <<interface>>
        +GetReceivedBytes()
        +TransmitData()
    }

    class ProtocolGeneral{

    }

    class ProtocolReceiver{

    }

    class ProtocolTransmitter{

    }

    ProtocolGeneral <|-- ProtocolReceiver
    ProtocolReceiver ..> IProtocolTransmission
    ProtocolReceiver ..> IPcCommunications

    ProtocolTransmitter --|> IProtocolTransmission
    ProtocolTransmitter --|> ProtocolGeneral
    ProtocolTransmitter ..> IPcCommunications
```