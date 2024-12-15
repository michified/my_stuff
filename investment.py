import yfinance as yf
import pandas as pd
import datetime as dt

timenow = dt.datetime.now()
time2db = timenow - dt.timedelta(3)

tickers = pd.read_html('https://en.wikipedia.org/wiki/List_of_S%26P_500_companies')[0]

candidates = []
names = tickers.Symbol.to_list()
datas = yf.download(names, period='7d', interval='30m')['Close']

for name in names:
    data = datas[name]
    if data.empty:
        continue

    compare = []  # close, sma, bb lower, bb upper
    for d in data:
        compare.append([d])

    x = 0

    for sma in data.rolling(window=20).mean():
        compare[x].append(sma)
        x += 1

    x = 0

    for rstd in data.rolling(window=20).std():
        compare[x].append(compare[x][1] - (2 * rstd))
        compare[x].append(compare[x][1] + (2 * rstd))
        x += 1

    n = len(compare)

    if compare[n - 1][0] < compare[n - 1][1]:
        continue

    bbtouch = False
    touchtime = 0
    diptime = 0
    time = 1

    for i in range(n - 1, n - 10, -1):
        if compare[i][0] < compare[i][1]:
            diptime = min(time, diptime)
        if compare[i][0] < compare[i][2]:
            bbtouch = True
            touchtime = time
            break
        time += 1

    if bbtouch:
        candidates.append([touchtime + diptime -
                           ((compare[n - 1][0] - compare[n - 1][3]) / compare[n - 1][3]) * 100, name])

candidates.sort()

for i in range(100):
    print()

print('Recommended investments:')

a = len(candidates)

if a == 0:
    print('None as of now')
else:
    for i in range(len(candidates)):
        if candidates[i][0] == float('nan'):
            a = min(a, i)
            print(candidates[i][1], end=' | ')

    for i in range(a - 1):
        if candidates[i][0] != float('nan'):
            print(candidates[i][1], end=' | ')
    print(candidates[a - 1][1])

for i in range(8):
    print()
