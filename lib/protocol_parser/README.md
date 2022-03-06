# Protocol Parser
This module parses packets recieved from the PC to enact change on the EEG data acquisition or pushed EEG data packets to the PC.

## Inheritance
This class publicly inherits from the EegDataConsumer class in order to consume EEG data, it uses the Protocol Frame Parser module to wrap the EEG data, and passes it on to the PC.