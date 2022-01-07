import pandas as pd
import json
import sys

if len(sys.argv) < 3:
    print("Not enough arguments.")
    print("<Community .csv file> <.json file>")
    exit(1)

community = list((pd.read_csv(sys.argv[1], usecols=[0], names=['0']))['0'])
raw = json.load(open(sys.argv[2]))
meta = (pd.DataFrame(raw['meta']))['address']

for vertex in community:
    print(meta[vertex])