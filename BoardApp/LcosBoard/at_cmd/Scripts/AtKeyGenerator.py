#!/c/Users/APPO/AppData/Local/Programs/Python/Python38/python.exe
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 26 09:49:49 2020

@author: GeDongrui
"""
import io
import os
import sys
import numpy
from math import sqrt
import AtSupport as Ats
print("AtKey HashTable Generator")

atnum = 0  # AT命令个数
fillrate = 0.5  # 填充因子


keylist = []
cmdlist = []
script_path = os.path.dirname(os.path.abspath(__file__))
print(script_path)
cmdlist, keylist = Ats.IAtParamGet(script_path + "/../at_cmd.csv")


###### Main Function #####
atnum = len(keylist)
print("atnum = " + str(atnum))

#  Storage Hash Key & Value in "numpy struct"
HashType = numpy.dtype({
    'names': ['name', 'index', 'value'],
    'formats': ['S32', 'i', 'i']
})
hashtable = numpy.array([('AT', 0, 0)] * atnum, dtype=HashType)

for i, atstr in enumerate(keylist):
    value = Ats.BkdrHash(atstr)
    print("No." + str(i) + " str = " + atstr +
          ", "+", val = " + str(value))
    hashtable[i]['value'] = value
    hashtable[i]['name'] = atstr

""" check for repeat hash value """
value_list = []
for i in range(0, atnum):
    if hashtable[i]['value'] != 0:
        print(hex(i), hex(hashtable[i]['value']),
              hashtable[i]['name'].decode("utf-8"))
        value_list.append(hashtable[i]['value'])

dic = {}.fromkeys(value_list)
if len(dic) != atnum:
    print("ERROR! find out repeat value.")
    exit(-1)
else:
    print("hash check value ok.")

#  Create 'C' Source & Header File


class ctemplate:
    __fileName = ' '
    __sourceName = ' '
    __headerName = ' '

    def __init__(self, s):
        self.__filePath = os.path.dirname(os.path.abspath(__file__))
        self.__fileDir = os.path.dirname(
            self.__filePath)
        self.__fileName = s
        self.__sourceName = self.__fileDir + "\\" + s + ".c"
        self.__headerName = self.__fileDir + "\\" + s + ".h"

    def createSource(self):
        fh = io.open(self.__sourceName, mode='w', encoding='utf-8')
        cm = "#include " + "\"" + self.__fileName + ".h\"" + "\n"*2
        cm += ("unsigned short g_atkey_num = %d;\t\t/* AtKey Num */\n" % atnum)
        cm += ("asKeyListUnit gsKeyList[] = {\n")
        for i in range(0, atnum):
            atstr = "/*" + "{:02d}".format(i) + "*/" + \
                "{ " + "{:#010X}".format(hashtable[i]['value']) + ", 0 ,"
            cm += atstr
            if(hashtable[i]['value'] != 0):
                atstr = ("\"")+hashtable[i]['name'].decode("utf-8")+("\"")
                cm += atstr
            else:
                cm += ("NULL")
            atstr = "},"
            cm += atstr
            if(hashtable[i]['value'] != 0):
                atstr = " /* " + hashtable[i]['name'].decode("utf-8") + " */\n"
                cm += atstr
            else:
                cm += ("\n")
        cm += ("};\n")
        cm += ("uint16_t gSkey_list_len = sizeof(gsKeyList) / sizeof(asKeyListUnit);\n")
        fh.write(cm)
        fh.close()

    def createHeader(self):
        fh = io.open(self.__headerName, mode='w', encoding='utf-8')
        cm = "#ifndef __%s_H__\n" % self.__fileName.upper()
        cm += "#define __%s_H__\n" % self.__fileName.upper()
        cm += ("\n")
        cm += ("#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")
        cm += ("#include \"Src/At_internal/AtInternalConfig.h\"\n")
        cm += ("typedef enum __aeKeyName{\n")
        for i in range(0, atnum):
            if(hashtable[i]['value'] != 0):
                cm += "\t"
                atstr = "kKey" + \
                    hashtable[i]['name'].decode("utf-8") + " = " + hex(i)
                cm += atstr
                cm += (",\n")
        cm += ("} aeKeyName;\n\n")
        cm += ("extern unsigned short g_atkey_num;\n")
        cm += ("extern asKeyListUnit gsKeyList[];\n")
        cm += ("extern uint16_t gSkey_list_len;\n")
        cm += ("\n"*2)
        cm += ("#ifdef __cplusplus\n}\n#endif\n\n")
        cm += ("#endif\n")
        fh.write(cm)
        fh.close()

    def createTemplatePairs(self):
        self.createSource()
        self.createHeader()


if __name__ == '__main__':
    ct = ctemplate("AtKeyHashTable")
    ct.createTemplatePairs()
