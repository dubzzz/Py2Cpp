#!/usr/bin/python
from to_cpp_classes import *

# Based on Python structs

distance({'x': 0, 'y': 2}, {'x': 1, 'y': 3})
length(([{'x': 0, 'y': 2}, {'x': 1, 'y': 3}, {'x': 0, 'y': 2}],))

#Based on Python classes

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

distance(Point(0,0), Point(5,1))

