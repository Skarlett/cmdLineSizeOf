from os import path, getcwd
from sys import argv
from math import floor, log, pow

def convertSize(size):
   if (size == 0):
       return '0B'
   size_name = ("B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB")
   i = int(floor(log(size, 1024)))
   p = pow(1024,i)
   s = round(size/p,2)
   return '%s %s' % (s,size_name[i])

print(convertSize(path.getsize(path.join(getcwd(), argv[1]))))
