import pygame as pg
import os
import sys

pg.init()

width = 7
height = 6
cmpBitBoard = 0
hmnBitBoard = 0
numPieces = [0] * width
bitwiseDirs = [1, 7, 8, 6]

def addPieceCmp(col):
    global cmpBitBoard
    cmpBitBoard ^= 1 << (col * 7 + numPieces[col])
    numPieces[col] += 1

def addPieceHmn(col):
    if numPieces[col] == height:
        return False
    global hmnBitBoard
    hmnBitBoard ^= 1 << (col * 7 + numPieces[col])
    numPieces[col] += 1
    return True

def isWin():
    for direction in bitwiseDirs:
        if (cmpBitBoard & (cmpBitBoard >> direction) & (cmpBitBoard >> (direction * 2)) & (cmpBitBoard >> (direction * 3))) != 0:
            return 1
    for direction in bitwiseDirs:
        if (hmnBitBoard & (hmnBitBoard >> direction) & (hmnBitBoard >> (direction * 2)) & (hmnBitBoard >> (direction * 3))) != 0: 
            return -1
    return 0
    
SQUARESIZE = 100
WINDOWWIDTH = SQUARESIZE * width
WINDOWHEIGHT = SQUARESIZE * (height + 1)
BLUE = (0, 0, 200)
YELLOW = (200, 200, 0)
TRANSPARENTYELLOW = (59, 59, 2)
RED = (200, 0, 0)
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
CIRCLERADIUS = SQUARESIZE / 2 - 10
windowsize = (WINDOWWIDTH, WINDOWHEIGHT)
font = pg.font.SysFont("monospace", 50)

def drawBoard():
    row = 1
    rowNum = 0
    while row <= (1 << 5):
        cell = row
        colNum = 0
        while cell <= (1 << 47):
            x = colNum * SQUARESIZE
            y = WINDOWHEIGHT - (rowNum + 1) * SQUARESIZE
            pg.draw.rect(screen, BLUE, (x, y, SQUARESIZE, SQUARESIZE))
            if cmpBitBoard & cell:
                pg.draw.circle(screen, RED, (x + SQUARESIZE / 2, y + SQUARESIZE / 2), CIRCLERADIUS)
            elif hmnBitBoard & cell:
                pg.draw.circle(screen, YELLOW, (x + SQUARESIZE / 2, y + SQUARESIZE / 2), CIRCLERADIUS)
            elif not (x + SQUARESIZE / 2, y + SQUARESIZE / 2) == prevCoords:
                pg.draw.circle(screen, BLACK, (x + SQUARESIZE / 2, y + SQUARESIZE / 2), CIRCLERADIUS)
            else:
                pg.draw.circle(screen, TRANSPARENTYELLOW, prevCoords, CIRCLERADIUS)
            cell <<= 7
            colNum += 1
        row <<= 1
        rowNum += 1
    pg.display.update()
    
gameOver = False
winner = 0
placed = 0
prevCoords = (-1, -1)

inp = input("Go first? y/n\n")
if inp == 'n':
    addPieceCmp(3)
    placed += 1

screen = pg.display.set_mode(windowsize)

while not gameOver:
    drawBoard()
    for event in pg.event.get():
        if event.type == pg.MOUSEMOTION:
            curCol = event.pos[0] // SQUARESIZE
            curCenter = curCol * SQUARESIZE + SQUARESIZE // 2
            pg.draw.rect(screen, BLACK, (0, 0, WINDOWWIDTH, SQUARESIZE))
            pg.draw.polygon(screen, YELLOW, [[curCenter, SQUARESIZE * 3 // 4], [curCenter - SQUARESIZE // 4, SQUARESIZE // 4], [curCenter, SQUARESIZE // 2.5], [curCenter + SQUARESIZE // 4, SQUARESIZE // 4]])
            if prevCoords[0] != -1 and numPieces[curCol] < height:
                pg.draw.circle(screen, BLACK, prevCoords, CIRCLERADIUS)
            prevCoords = (curCenter, (WINDOWHEIGHT - ((numPieces[curCol] + 1) * SQUARESIZE)) + SQUARESIZE // 2)
            pg.draw.circle(screen, TRANSPARENTYELLOW, prevCoords, CIRCLERADIUS)
            pg.display.update()
            
        if event.type == pg.quit:
            sys.exit()
            
        if event.type == pg.MOUSEBUTTONDOWN:
            if addPieceHmn(event.pos[0] // SQUARESIZE):
                drawBoard()
                placed += 1
                if isWin() or placed == width * height:
                    winner = isWin()
                    gameOver = True
                    if winner == 1:
                        label = font.render("boo hoo you lost ;(", True, WHITE)
                    elif winner == -1:
                        label = font.render("nice you won :)", True, WHITE)
                    else:
                        label = font.render("tie", True, WHITE)
                    screen.blit(label, (40, 10))
                    pg.display.update()
                    pg.time.wait(3000)
                    break
                oldCmpBoard = cmpBitBoard
                pg.draw.rect(screen, BLACK, (0, 0, WINDOWWIDTH, SQUARESIZE))
                screen.blit(font.render("computer is thinking...", True, WHITE), (40, 10))
                pg.display.update()
                addPieceCmp(os.system("cppfile.exe " + str(cmpBitBoard) + " " + str(hmnBitBoard)))
                pg.draw.rect(screen, BLACK, (0, 0, WINDOWWIDTH, SQUARESIZE))
                pg.display.update()
                for i in range(2):
                    drawBoard()
                    oldCmpBoard, cmpBitBoard = cmpBitBoard, oldCmpBoard
                    pg.time.wait(200)
                    drawBoard()
                    pg.time.wait(200)
                    oldCmpBoard, cmpBitBoard = cmpBitBoard, oldCmpBoard
                drawBoard()
                placed += 1
                if isWin() or placed == width * height:
                    winner = isWin()
                    gameOver = True
                    if winner == 1:
                        label = font.render("boo hoo you lost ;(", True, WHITE)
                    elif winner == -1:
                        label = font.render("nice you won :)", True, WHITE)
                    else:
                        label = font.render("tie", True, WHITE)
                    screen.blit(label, (40, 10))
                    pg.display.update()
                    pg.time.wait(3000)
                    break