import os
import sys
import struct
def u32(x):
    return struct.unpack(">I",x)[0]
def p32(x):
    return struct.pack(">I",x)


with open(sys.argv[1],"rb") as f:
    data = f.read()

output = b""
for i in range(0,len(data),4):
    a = u32(data[i:i+4])
    output += p32(a^0xd3abc0de)

output += p32(0)*2
with open(sys.argv[2],"wb+") as f:
    f.write(output)

