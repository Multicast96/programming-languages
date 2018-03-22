import pygame
import time
import sys


WINDOW_WIDTH = 640
WINDOW_HEIGHT = 480

canvas = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))

deltaTime = 0
getTicksLastFrame = 0

position = (100, 100)
r1 = pygame.Rect(position, (100, 100))


clk = pygame.time.Clock()
while True:
    clk.tick(60)
    # Events handler
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            sys.exit(0)


    # Clear screen
    canvas.fill((0, 0, 0))

    pygame.draw.rect(canvas, (234, 100, 140), r1)
    # Display
    pygame.display.flip()




