#!/usr/bin/env python3

import sys
import os
import re

if len(sys.argv) != 2:
    print("\tUsage:%s src"%sys.argv[0])
    exit(0)

try:
    with open(sys.argv[1],"r") as f:
        data = f.read()
except:
    print("not found file")
    exit(0)

def get_uuid(a):
    return os.path.basename(a).replace(".","_")

def get_dst_file(a):
    return open(a.replace(".s","_new.s"),"w+")

def get_obj_file(a):
    return open(a.replace(".s","_obf.s"),"w+")

uuid = get_uuid(sys.argv[1])

dst_file = get_dst_file(sys.argv[1])
obf_file = get_obj_file(sys.argv[1])

label_m = re.compile("\$L[0-9a-zA-Z]*")
def replace_label(a):
    b = a
    for label in label_m.findall(a):
        b = b.replace(label,"_%s_%s"%(uuid,label[1:]))
    return b

asm = data.split("\n")

global_name = "__"+uuid
cnt = 0
for line in asm:
    line = line.strip()
    if line == "":
        continue
    line = replace_label(line)

    if line.startswith("_") and line.endswith(":"):
        label = line[:-1]
        dst_file.write(".global %s\n"%label)
        dst_file.write(line+"\n")
        continue
    if not line.startswith("j") and not line.startswith("b"):
        dst_file.write(line+"\n")
        continue
    prefix =  "%s__%d"%(global_name,cnt)
    func_begin = prefix + "_begin"
    func_jmp = prefix + "_jmp"
    func_ret = prefix + "_ret"
    new_code = """
    .global %s
        mtc0 $0,$9
    %s:
        b .
    .global %s
    %s:
    """%(func_begin,func_begin,func_ret,func_ret)
    cnt += 1
    dst_file.write(new_code)
    obf_code = """
    .section .data.obf
    .word %s
    .word %s
    .section .text.obf
    .global %s
    %s:
    %s
    b %s
    nop
    """%(func_begin,func_jmp,func_jmp,func_jmp,line,func_ret)
    obf_file.write(obf_code)



dst_file.close()
obf_file.close()

