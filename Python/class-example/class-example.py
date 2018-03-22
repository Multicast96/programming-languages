from abc import abstractmethod, ABC
from enum import Enum


# Interface / abstract class
class Drivable(ABC):
    class Directions(Enum):
        LEFT = 0
        RIGHT = 1
        FORWARD = 2
        BACKWARD = 3
        UP = 4
        DOWN = 5

    speed = 0
    direction = Directions.FORWARD
    ready_to_run = False

    @abstractmethod
    def change_speed(self, delta): pass

    @abstractmethod
    def turn(self, direction): pass


class Vehicle(ABC):
    def __init__(self, weight, length):
        self.weight = weight
        self.length = length

    @abstractmethod
    def start(self): pass


class Bike(Vehicle, Drivable):
    current_left_gear = 0
    current_right_gear = 0

    def __init__(self, weight, length, left_gears, right_gears):
        super().__init__(weight, length)
        self.LGears = left_gears
        self.LGears = right_gears

    def start(self):
        print("Starting "+self.__class__.__name__+" ride !!")

    def turn(self, direction):
        if direction == "L":
            self.direction = self.Directions.LEFT
            print("Turning left")
        elif direction == "R":
            self.direction = self.Directions.RIGHT
            print("Turning right")

    def change_speed(self, delta):
        print("Current speed: "+str(self.speed)+" new: "+str(self.speed+delta));
        self.speed += delta


b = Bike(2, 1.3, 3, 5)
b.start()
b.change_speed(23);
b.turn("L")



