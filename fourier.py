import pygame as pg
from pygame import gfxdraw
import cmath as cm
from functools import cmp_to_key

YELLOW = (255, 255, 40)
WHITE = (255, 255, 255)
BLUE = (41, 112, 255)
BLACK = (0, 0, 0)
DARKYELLOW = (74, 63, 0)

pg.init()
infoObject = pg.display.Info()
gameDisplay = pg.display.set_mode((infoObject.current_w, infoObject.current_h))

brush = None
prevb = None
game_exit = False

strokes = []
curStroke = []
posArray = []

while not game_exit:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            game_exit = True

        elif event.type == pg.MOUSEBUTTONDOWN:
            if event.button == 1:
                brush = event.pos
                prevb = event.pos
        elif event.type == pg.MOUSEBUTTONUP:
            if event.button == 1:
                brush = None
        elif event.type == pg.MOUSEMOTION:
            if brush:
                brush = event.pos

    if brush:
        pg.draw.line(gameDisplay, YELLOW, (prevb[0], prevb[1]), (brush[0], brush[1]), 5)
        curStroke.append(complex(brush[0], brush[1]))
        prevb = brush
    elif len(curStroke) > 0:
        strokes.append(curStroke)
        curStroke = []
             
    pg.display.flip()
pg.quit()

for stroke in strokes:
    for elem in stroke:
        posArray.append(elem)

def calcAngle(n):
    if n.real > 0 and n.imag >= 0: 
        return cm.atan(abs(n.imag) / abs(n.real))
    if n.real < 0 and n.imag >= 0:
        return cm.pi - cm.atan(abs(n.imag) / abs(n.real))
    if n.real < 0 and n.imag < 0:
        return cm.pi + cm.atan(abs(n.imag) / abs(n.real))
    if n.real > 0 and n.imag < 0:
        return cm.pi * 2 - cm.atan(abs(n.imag) / abs(n.real))
    if n.imag >= 0:
        return cm.pi / 2
    return cm.pi * 3 / 2

def pos(c, n, t):
    return c * cm.exp(complex(0, n * cm.pi * 2 * t))

spd = int(input("Drawing speed coefficient? The larger your drawing, the higher you need to set this "))
tol = float(input("How fast did you draw? 1 - 3 : slow - fast; input inf if only drew 1 line "))
cnt = int(input("How many rotating terms? Recommended: low: ~10, medium: ~20, high: ~40, max: " + str(len(posArray)) + " "))

def dft(points):
    res = []
    n = len(points)
    for k in range(n):
        tot = complex(0, 0)
        for t in range(n):
            theta = cm.pi * -2 * k * t / n
            c = cm.exp(complex(0, theta))
            tot += points[t] * c
        tot /= n
        res.append([k, tot, (tot.real ** 2 + tot.imag ** 2) ** 0.5, calcAngle(tot)])
        if k % 100 == 0:
            print(round(k / n * 100, 1), "percent done calculating")
    return res
    
dials = dft(posArray)
dials = sorted(dials, key=cmp_to_key(lambda a, b: b[2] - a[2]))

gameDisplay = pg.display.set_mode((infoObject.current_w, infoObject.current_h))

cur = None
t = 0
dt = cm.pi * 2 / (len(dials) / spd)
been = [[]]
clock = pg.time.Clock()
    
def drawVector(prev, cur, ind):
    if ind < 20:
        pg.draw.circle(gameDisplay, WHITE, (prev.real, prev.imag), 
                 int(cm.log(cm.log((cur.real - prev.real) ** 2 + (cur.imag - prev.imag) ** 2)).real ** 2) // 2)
    pg.draw.line(gameDisplay, WHITE, (prev.real, prev.imag), ((cur - (cur - prev) / 15).real, (cur - (cur - prev) / 15).imag), 
                 int(cm.log(cm.log((cur.real - prev.real) ** 2 + (cur.imag - prev.imag) ** 2)).real ** 2))
    perp = (cur - prev) * complex(cm.cos(cm.pi / 2), cm.sin(cm.pi / 2))
    p1 = cur - (cur - (prev + perp / (3 ** 0.5))) / 10
    p2 = cur - (cur - (prev - perp / (3 ** 0.5))) / 10
    gfxdraw.aapolygon(gameDisplay, [(cur.real, cur.imag), (p1.real, p1.imag), (p2.real, p2.imag)], WHITE)
    pg.draw.polygon(gameDisplay, WHITE, [(cur.real, cur.imag), (p1.real, p1.imag), (p2.real, p2.imag)])    
    
def drawEpicycles(cur, rot):
    i = 0
    for dial in dials:
        if i > cnt:
            break
        i += 1
        prev = cur
        cur += complex(dial[2] * cm.cos(dial[0] * t + dial[3] + rot), dial[2] * cm.sin(dial[0] * t + dial[3] + rot))
        if dial[0] != 0:
            gfxdraw.aaellipse(gameDisplay, int(prev.real), int(prev.imag), int(dial[2]), int(dial[2]), BLUE)
            gfxdraw.aaellipse(gameDisplay, int(prev.real), int(prev.imag), int(max(1, dial[2] - 2)), int(max(1, dial[2] - 2)), BLACK)
            drawVector(prev, cur, i)
        else:
            pg.draw.circle(gameDisplay, WHITE, (cur.real, cur.imag), 10)
            
    return complex(round(cur.real, 1), round(cur.imag, 1))

game_exit = False

while not game_exit:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            game_exit = True
    cur = complex(0, 0)
    angle = 0
    gameDisplay.fill(BLACK)
    toadd = drawEpicycles(0, 0)
    if (len(been[-1]) > 0 and abs(toadd.real - been[-1][-1].real) > (tol * 10) and abs(toadd.imag - been[-1][-1].imag) > (tol * 10)):
        been.append([toadd])
    else:
        been[-1].append(toadd)
    for segment in strokes:
        for i in range(len(segment) - 1):
            pg.draw.line(gameDisplay, DARKYELLOW, (segment[i].real, segment[i].imag), (segment[i + 1].real, segment[i + 1].imag), 4)
    for segment in been:
        for i in range(len(segment) - 1):
            pg.draw.line(gameDisplay, YELLOW, (segment[i].real, segment[i].imag), (segment[i + 1].real, segment[i + 1].imag), 4)
    t += dt
    if t > cm.pi * 2:
        t = 0
        been = [[]]
    pg.display.flip()
    
pg.quit()