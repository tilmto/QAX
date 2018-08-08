import re
import csv
import sys

fr=open(sys.argv[1],'r')
fm=open(sys.argv[2],'r')

benchr=[]
benchm=[]

fr_csv=csv.DictReader(fr)
for row in fr_csv:
	benchr.append(row['Gate Cost'])

fr.close()

fm_csv=csv.DictReader(fm)
for row in fm_csv:
	benchm.append(row['Gate Cost'])

fm.close()

if(len(benchr)!=len(benchm)):
	exit()

count=0;
for i in range(len(benchr)):
	if(benchm[i]<=benchr[i]):
		count=count+1

print(count)
