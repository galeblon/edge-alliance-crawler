import pandas as pd
import sys

if len(sys.argv) < 3:
    print("Not enought arguments.")
    print("<Community .csv file> <.json file>")
    exit(1)

community = list((pd.read_csv(sys.argv[1]))['0'])
raw = pd.read_json(sys.argv[2])
meta = raw.meta

for vertex in community:
    print(meta[vertex]['address'])