# coding=Big5

import re
import codecs

def extract_ZY():
    ZY=dict()  #create a dictionary to store ZhuYin match relationship

    with open ('Big5-ZhuYin.map','rb') as f:
        for line in f.readlines():
            #line = line.decode('big5-hkscs')
            line = line.decode('big5-hkscs','ignore')
            striped=line.strip()
            t=re.split(r'[/\s]',striped)  #split source data by '/' and space
            for i in range(1,len(t)): #loop from second item to last item in t list
                if t[i][0] in ZY:  #the first character of zhuyin
                    ZY[t[i][0]].append(t[0])
                else:
                    ZY[t[i][0]]=[t[0]]
    return ZY


def out(ZY):
    with codecs.open('ZhuYin-Big5.map', 'w', encoding='big5-hkscs') as out:
        for k, v in ZY.items():
            s=""
            #s+=str(k)+"\t"
            s+=k+"\t"
            unique=list(set(v))
            for i in range(len(v)):
                #s+=str(v[i])
                s+=v[i]
                s+=" "
            out.write(s)
            out.write("\n")
            for x in range(len(unique)):
                s=""
                #s=str(unique[x])+"\t"+str(unique[x])
                s=unique[x]+"\t"+unique[x]
                out.write(s)
                out.write("\n")


if __name__ == '__main__':
    ZY=extract_ZY()
    out(ZY)
