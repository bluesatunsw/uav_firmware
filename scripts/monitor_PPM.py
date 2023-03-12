"""
Script to read in serial from the rp2040 during prototyping

NOTE:
Channels are not updated from main src file,
so please check that the right channel number 
corresponds to the correct input before use.
"""

# Imports
import serial.tools.list_ports as list_ports
from serial import Serial
from json import dumps
from math import floor
from dataclasses import dataclass, field
from functools import total_ordering


# Constants
BAUD_RATE = 4800
MIN_VALUE = 968
MAX_VALUE = 1992
VALUE_SCALE_FACTOR = 8
ENCODING = 'ascii'
CHANNELS = {
    'Yaw': 0,
    'Pitch': 1,
    'Throttle': 2,
    'Roll': 3,
    'TwoWay': 4,
    'ThreeWay1': 5,
    'ThreeWay2': 6,
    'Potentiometer': 7
}
SWITCH_NO_CHANGE = 1


# Channel Classes
@total_ordering
@dataclass
class Channel:
    percentage: int = field()
    name:       str = field(init=False)
    id:         int = field(init=False)
    
    def __str__(self) -> str:
        return str(self.value)
    
    def _operator_fallbacks(monomorphic_operator):
        # Will likely only need forward pass as another compatible
        # class would never be expected to be made
        def forward(a, b):
            try:
                return monomorphic_operator(a, b)
            except ValueError:
                return NotImplemented
        
        return forward
    
    @_operator_fallbacks
    def __eq__(self, __o: object) -> bool:
        return self.value == __o.value
    
    @_operator_fallbacks
    def __lt__(self, __o: object) -> bool:
        return self.value < __o.value
    
    @_operator_fallbacks
    def __sub__(self, __o: object) -> int:
        return self.value - __o.value
    
    @property
    def value(self):
        return self.percentage
    
    @property
    def _has_changed(self):
        return self.value != 0
    

class ChannelCenterDefault(Channel):
    _offset = 50
    
    # def __post_init__(self):
    #     self.value = self.value - self._offset
    @property
    def value(self):
        return self.percentage - self._offset
    

class ChannelSwitch(Channel):
    _no_change_value = SWITCH_NO_CHANGE
    
    def __str__(self) -> str:
        return self.position
    
    @Channel._operator_fallbacks
    def __sub__(self, __o: object) -> int:
        """Subtract method for switches
        Returns new position if there is a change, 
        otherwise self.changed is set to False.

        Args:
            __o (object): Object

        Returns:
            int: Value of difference switch
        """
        if self.position == __o.position:
            return self._no_change_value
        else:
            return self.value
    
    @property
    def value(self):
        return self.position
    
    @property            
    def _has_changed(self):
        return self.percentage != self._no_change_value
            
    @property
    def position(self):
        if self.up:
            return 'Up'
        if self.center:
            return 'Center'
        if self.down:
            return 'Down'
        if not self.changed:
            return 'No Change'
        return 'INVALID POSITION??'
    
    @property
    def up(self):
        return self.percentage == 100
    
    @property
    def center(self):
        return self.percentage == 50
    
    @property
    def down(self):
        return self.percentage == 0
    
    @property
    def changed(self):
        return self._has_changed
    

class Yaw(ChannelCenterDefault):
    name = 'Yaw'
    id   = CHANNELS[name]
    

class Pitch(ChannelCenterDefault):
    name = 'Pitch'
    id   = CHANNELS[name]
    

class Throttle(Channel):
    name = 'Throttle'
    id   = CHANNELS[name]    


class Roll(ChannelCenterDefault):
    name = 'Roll'
    id   = CHANNELS[name]
    

class TwoWay(ChannelSwitch):
    name = 'TwoWay'
    id   = CHANNELS[name]
    

class ThreeWay1(ChannelSwitch):
    name = 'ThreeWay1'
    id   = CHANNELS[name]
    

class ThreeWay2(ChannelSwitch):
    name = 'ThreeWay2'
    id   = CHANNELS[name]
    

class Potentiometer(Channel):
    name = 'Potentiometer'
    id   = CHANNELS[name]
    

def unstable_initialiser(cls):
    def wrapper(*args):
        try:
            return cls(*args)
        except TypeError:
            print('Invalid Packet')
    return wrapper


@unstable_initialiser
class ChannelPacket:
    def __init__(
        self,
        yaw: int,
        pitch: int,
        throttle: int,
        roll: int,
        two_way: int,
        three_way_1: int,
        three_way_2: int,
        potentiometer: int
    ):
        self.yaw = Yaw(yaw)
        self.pitch = Pitch(pitch)
        self.throttle = Throttle(throttle)
        self.roll = Roll(roll)
        self.two_way = TwoWay(two_way)
        self.three_way_1 = ThreeWay1(three_way_1)
        self.three_way_2 = ThreeWay2(three_way_2)
        self.potentiometer = Potentiometer(potentiometer)
    
        self.channels = [
            self.yaw,
            self.pitch,
            self.throttle,
            self.roll,
            self.two_way,
            self.three_way_1,
            self.three_way_2,
            self.potentiometer
        ]
    
    def __repr__(self) -> str:
        return f'<{self.yaw}, {self.pitch}, {self.throttle}, {self.roll}, {self.two_way}, {self.three_way_1}, {self.three_way_2}, {self.potentiometer}>'
    
    def __iter__(self):
        return iter(self.channels)
    
    def _operator_fallbacks(monomorphic_operator):
        # Will likely only need forward pass as another compatible
        # class would never be expected to be made
        def forward(a, b):
            return monomorphic_operator(a, b)
        
        return forward

    @_operator_fallbacks
    def __eq__(self, __o: object) -> bool:
        for i, j in list(zip(self.channels, __o.channels)):
            if i != j:
                return False
        return True

    @_operator_fallbacks
    def __sub__(self, __o: object) -> bool:
        diff = []
        
        for i, j in list(zip(self.channels, __o.channels)):
            diff.append(i - j)
            
        return ChannelPacket(*diff)
    
    def get_changes(self, prev) -> list:
        changes = []
        
        for channel in (prev - self):
            if channel._has_changed:
                changes.append(self.channels[channel.id])
                
        return changes
    
    @classmethod
    def generate_empty():
        return ChannelPacket(0,0,0,0,0,0,0,0)


# Operation Functions
def main():
    """
    Handles the main event loop to monitor serial port
    """
    port = get_comport()
    
    with Serial(port, BAUD_RATE) as ser:
        serial_loop(ser)


def get_comport() -> str:
    """Gets the comport 

    Returns:
        str: port name
    """
    ports = list_ports.comports()
    print(f'Available Ports: {ports}')
    port = ''
    
    while port not in ports:
        port = input('Select Port: ')
        if port not in ports:
            print('Port not recognised, try again')
    
    return port


def serial_loop(ser: Serial):
    prev = ChannelPacket.generate_empty()
    
    while True:
        data = read_data(ser)
        
        for change in data.get_changes(prev):
            print(f'Changes {change.name} to {change}')
    

def read_data(ser: Serial) -> ChannelPacket:
    """Reads in a data line from serial and outputs as a percentage int

    Args:
        ser (Serial): Open serial
    """
    # Read in raw values
    raw_values = ser.readline().decode(ENCODING).split(' ')
    
    # Adjust them to a range [0, 128] (will also remove +-4 oscillations, hence floor)
    adj_values = [floor((x - MIN_VALUE) / 8) for x in raw_values]
    
    # Normalise values and make percentage
    perc_values = [x / (MAX_VALUE - MIN_VALUE) * 100 for x in adj_values]
    
    # Create a channel packet object and return
    return ChannelPacket(*perc_values)


if __name__ == '__main__':
    # Print the current settings and config
    # so the user can check that they are correct
    print(f'Using Baud Rate {BAUD_RATE}')
    print(f'Channels in order are:')
    for channel, i in CHANNELS.items():
        print(f'Channel {i}: {channel}')
    input('Press Enter to Confirm')
    
    # Run main event loop
    main()

