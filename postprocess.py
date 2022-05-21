import pandas as pd
import argparse

parser = argparse.ArgumentParser(description="Parameters")
parser.add_argument('--p', type=str, default='clustermaps_level1/fao_eggplant_2000_level1',
                    help='Dataset path (default: clustermaps_level1/fao_eggplant_2000_level1')
args = parser.parse_args()
path = args.p


df = pd.read_csv(path,sep=' ', names=['Id','CommId'], index_col='Id')
country = pd.read_csv('country_map.csv', sep='|', names=['Name', 'ISOCode', 'Id'], index_col='Id')
#print(df.head())
#print(country.head())
joined = country.join(df, rsuffix='lid')
#print(joined.head())
joined.to_csv('postprocessed/Country_'+path.split('/')[1][:-7]+'.csv', index_label='CountryId')
