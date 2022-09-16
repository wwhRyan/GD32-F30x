# -*- coding: utf-8 -*-
"""
Created on Sun Oct 25 16:34:01 2020

@author: APPO
"""
import pandas as pd
from math import sqrt


def IAtParamGet(filepath):
    # sheetname='AT_CMD'
    data = pd.read_csv(filepath, header=0, usecols=[0, 1, 3])
    cmd = set()
    key = set()
    for j in data.index:
        temp_cmd = data.loc[j].CMD
        temp_key = data.loc[j].KEY
        temp_value = data.loc[j].VALUE
        if(isinstance(temp_cmd, str)):
            cmd.add(temp_cmd)
        if(isinstance(temp_key, str)):
            key.add(temp_key)
        if(isinstance(temp_value, str)):
            key.add(temp_value)
    # key.discard('Status')
    cmd_list = list(cmd)
    key_list = list(key)
    # cmd_list.sort()
    # key_list.sort()
    return cmd_list, key_list

#  Judging Prime


def IsPrime(num):
    if num > 1:
        if num == 2:
            return True
        if num % 2 == 0:
            return False
        for i in range(3, int(sqrt(num) + 1), 2):
            if num % i == 0:
                return False
        return True
    return False

#  Find Minimum Prime over than "num"


def MinPrime(num):
    while True:
        if(IsPrime(num) == True):
            break
        else:
            num = num + 1
    return num

#  Hash Function


def BkdrHash(input_str):
    seed = 13131  # 31 131 1313 13131 131313 etc..
    hashcode = 0
    for char in input_str:
        hashcode = hashcode * seed + ord(char)
    return (hashcode & 0x7FFFFFFF)

#  Conflict Detection Function


def ReHash(key, hashtable, hash_table_len):
    index = key
    delta = index % 7 + 1  # 7 is a prime number, +1 to avoid delta = 0.
    i = 0
    while (i < hash_table_len) and (hashtable[index]['value'] != 0):
        print("dirty index = " + str(index) + "collide time: " + str(i))
        index = (index + delta) % hash_table_len
        i += 1
    if (i < hash_table_len):
        return index, i
    else:
        return -1, 0
