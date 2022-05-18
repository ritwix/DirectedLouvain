import pandas as pd
import sys

path = sys.argv[1]

df = pd.read_csv(path)

df = df[['src', 'dest', 'weight']]

df['weight'] = df['weight'].astype(int)

network_name = path.split('/')[-1][:-4]
print('Network:',network_name)
df.to_csv('./food_networks/'+network_name+'.txt', sep=' ', header=False, index=False)
