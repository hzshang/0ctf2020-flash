import sys
import struct

FLAG= b"flag{it's_time_to_pwn_this_machine!}"
def u16(x):
    return struct.unpack(">H",x)[0]
def p16(x):
    return struct.pack(">H",x)

def extend_gcd(a,b):
    if b==0:
        return a,1,0
    else:
        g,s,t=extend_gcd(b,a%b)
        return g,t,s-a//b*t


def reverse(a,N):
    return extend_gcd(a,N)[1]
 
if len(sys.argv) != 2:
    exit(0)

flag = FLAG[5:-1]

rsa_p = 653
rsa_q = 71
rsa_e = 17
rsa_mod = (rsa_p-1)*(rsa_q-1)
rsa_d = reverse(rsa_e,rsa_mod)

def do_rsa_enc(x):
    return (x*rsa_e)%rsa_mod

def do_rsa_dec(x):
    return (x*rsa_d)%rsa_mod

assert do_rsa_dec(do_rsa_enc(0x1234)) == 0x1234

if len(flag)%2:
    flag += b"\x00"

flag_num = []

for i in range(0,len(flag),2):
    flag_num.append(u16(flag[i:i+2]))

cmp_num = list(map(do_rsa_enc,flag_num))



code  = """
    push 2333
start:
    push 0
    push regs[0]
    cmpe
    je do_rsa
    push 1
    push regs[0]
    sub
    pop regs[0]        # length -= 1
    push in            # push data
    push 1
    je start
do_rsa:
"""

rsa_part = """
                 # (x*e) mod N
    push %d      # e
    mul
    push %d      # N
    mod
    push %d      # cmp data
    cmpe
    jne die
"""

end = """
success:
    push 2333
    jne die
    push 0
    pop regs[0]
    hlt

die:
    push 1
    pop regs[0]
    hlt
"""

for i in cmp_num[::-1]:
    code += rsa_part%(rsa_e,rsa_mod,i)

code += end

lines = code.split("\n")
labels = {}
idx = 0

def parse_line(line):
    end = line.find("#")
    if end > 0:
        line = line[0:end]
    ret = line.lstrip().rstrip().split(" ")
    if ret[0] == "":
        return None
    if ret[0].endswith(":"):
        return ["label",ret[0][:-1]]
    return ret

# 0 +
# 1 -
# 2 *
# 3 %
# 4 cmp less
# 5 cmp equal
# 6 jmp if true
# 7 jmp if false
# 8 push in
# 9 push const
# 10 push reg
# 11 pop reg
# 12 pop
# 13 stop
bin_code = b""
relocate_table = {}
for line in lines:
    op= parse_line(line)
    if op == None:
        continue
    if op[0] == "label":
        labels[op[1]] = idx
    if op[0] == "push":
        if op[1] == "regs[0]":
            bin_code += p16(10)
        elif op[1] == "in":
            bin_code += p16(8)
        else:
            num = int(op[1])
            bin_code += p16(9) + p16(num)
    if op[0] == "pop":
        if len(op) == 1:
            bin_code += p16(12)
        if op[1] == "regs[0]":
            bin_code += p16(11)
    if op[0] == "hlt":
        bin_code += p16(13)
    if op[0] == "cmpl":
        bin_code += p16(4)
    if op[0] == "cmpe":
        bin_code += p16(5)
    if op[0] == "mul":
        bin_code += p16(2)
    if op[0] == "mod":
        bin_code += p16(3)
    if op[0] == "sub":
        bin_code += p16(1)
    if op[0] == "add":
        bin_code += p16(0)
    if op[0] == "je":
        bin_code += p16(6) + b"\xff\xff"
        relocate_table[idx+2] = op[1]
    if op[0] == "jne":
        bin_code += p16(7) + b"\xff\xff"
        relocate_table[idx+2] = op[1]
    idx = len(bin_code)

bin_array = bytearray(bin_code)

# relocate
for idx,label in relocate_table.items():
    dst_idx = labels[label]
    dist = ((dst_idx - idx - 2)+0x10000)&0xffff
    # if dst_idx < idx + 2:
    #     print("dist: %x, dist>>8 %x"%(dist,dist >> 8))
    assert bin_array[idx] == 0xff
    assert bin_array[idx+1] == 0xff
    bin_array[idx] = (dist>>8)&0xff
    bin_array[idx+1] = dist&0xff


f = open(sys.argv[1],"w+")
f.write("""
.section .vm.code
.global vm_code
vm_code:
""")

for i in bin_array:
    f.write(".byte %d\n"%i)

f.close()










