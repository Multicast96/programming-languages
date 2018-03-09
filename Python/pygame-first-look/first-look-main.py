import pygame
import sys
import time
import random

WINDOW_WIDTH = 640
WINDOW_HEIGHT = 480

canvas = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
box_position = (100, 100)
box_size = 50
left_button_clicked = False


class Box(pygame.Rect):
    # ((left, top), (width, height))
    def __init__(self, left_top, width_height):
        super().__init__(left_top, width_height)
        self.n_left = float(left_top[0])
        self.n_top = float(left_top[1])
        self.n_width = float(width_height[0])
        self.n_height = float(width_height[1])
        self.n_center = (self.n_left + self.n_width/2, self.n_top+self.n_height/2)

    def move(self, x, y):
        self.n_center = (self.n_center[0] + x, self.n_center[1] + y)
        self.center = self.n_center

    def move_to(self, new_position):
        self.n_center = new_position
        self.center = new_position
boxes = []

while True:
    # Events handler
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            sys.exit(0)
        elif event.type == pygame.MOUSEBUTTONDOWN:
            left_button_clicked = True
            box_position = pygame.mouse.get_pos()

            # Rect((left, top), (width, height))
            boxes.append(Box(box_position, (box_size, box_size)))

        elif event.type == pygame.MOUSEMOTION:
            box_position = pygame.mouse.get_pos()
        elif event.type == pygame.MOUSEBUTTONUP:
            left_button_clicked = False

    # Input
    if left_button_clicked and boxes:
        # boxes[-1] is last element
        boxes[-1].move_to(box_position)
        boxes[-1].n_width += 0.05
        boxes[-1].width = boxes[-1].n_width
        boxes[-1].n_height += 0.05
        boxes[-1].height = boxes[-1].n_height

    # Clear screen
    canvas.fill((0, 0, 0))

    # Drawing
    for i, box in enumerate(boxes):
        # Gravity
        if not left_button_clicked or i != len(boxes)-1:
            boxes[i].move(0, 0.1)

        if boxes[i].y > WINDOW_HEIGHT:
            del boxes[i]
        else:
            pygame.draw.rect(canvas, (43, 234, 123), boxes[i])

    # Display
    pygame.display.flip()
