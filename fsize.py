from os import path, getcwd, walk
from sys import argv
from math import floor, log, pow
from string import digits, ascii_letters

size_names = ("B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB")

debug = False

def convert2Bytes(data):
        d = ''
        s = ''
        xpnet = -1
        for x in data:
            if x in digits+'.':
                d += x
            elif x in ascii_letters:
                s += x
        if not len(s) == 0:
            if s.upper() in size_names:
              for x in size_names:
                xpnet += 1
                if s.upper() == x:
                    break
            else:
                raise TypeError()
        else:
            xpnet = 0
        if '.' in d:
          return float(d.split('.')[0])*1024**(xpnet)+float('0.'+d.split('.')[1])*1024**(xpnet)
        else: return float(d.split('.')[0])*1024**(xpnet)

def convertSize(size):
   if size == 0:
     return '0 B'
   i = int(floor(log(size, 1024)))
   p = pow(1024,i)
   s = round(size/p,2)
   return '%s %s' % (s,size_names[i])


if __name__ == '__main__' and not debug:
   _path = path.join(getcwd(), argv[1])
   bytescnt = 0
   weirddataflag = 0
   weirdDatacnt = 0
   if path.isdir(_path):
    for x in walk(_path):
      for i in x[2]:
        try:
          bytescnt += path.getsize(path.join(x[0], i))
        except OSError:
            weirddataflag = 1
            weirdDatacnt += 1
    print(convertSize(bytescnt))
    if weirddataflag:
      print('There was some weird OS errors, number of weird files: '+str(weirdDatacnt))
   elif path.isfile(_path):
    print(convertSize(path.getsize(_path)))

   else:
     
     try:
       try:
         print(convertSize(convert2Bytes(argv[1]+argv[2])))
       except IndexError:
         print(convertSize(convert2Bytes(argv[1])))
     except TypeError:
       print('unrecognized data type')
