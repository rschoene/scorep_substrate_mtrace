#!/usr/bin/python

import argparse
import re
import subprocess
from subprocess import Popen, PIPE, call

maps=[]

def parse_mmap(mmap_file):
    f=open(mmap_file)
    reg_string='^([0-9a-f]*)-([0-9a-f]*)\s*..x.\s*\S*\s*\S*\s*\S*\s*(.*)$'
    re.compile(reg_string)
    for line in f:
        m=re.match(reg_string, line)
        if m != None:
            new_addr={}
            new_addr['start']=int(m.group(1),16)
            new_addr['end']=int(m.group(2),16)
            new_addr['symbol']=m.group(3)
            maps.append(new_addr)
    f.close()

def alloc_stat(mtrace_file):
    f=open(mtrace_file)
    reg_string='^@\s+(\S+):(\S+)\s+\+\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)$'
    entries= {}
    for line in f:
        m=re.match(reg_string, line)
        if m != None:
            symbol=m.group(1)
            position=m.group(2)
            mem_addr=int(m.group(3),16)
            mem_size=int(m.group(3),16)
            if symbol+position not in entries:
                entries[symbol+position] = {}
                entries[symbol+position]['symbol'] = symbol
                entries[symbol+position]['position'] = position
                entries[symbol+position]['count'] = 0
            entries[symbol+position]['count']+=1

    reg_string2='^\((\S*)\)\[0x([0-9a-f]+)\]$'
    re.compile(reg_string2)
    reg_string3='^\[0x([0-9a-f]+)\]$'
    re.compile(reg_string3)
    for entry_x in entries:
        entry=entries[entry_x]
        m=re.match(reg_string2, entry['position'])
        if m != None:
        # already translated
            print(str(entry['count'])+' allocs at '+entry['symbol']+':'+m.group(1))
        else:
            m=re.match(reg_string3, entry['position'])
            if m != None:
                mem_addr=int(m.group(1),16)
                pos_caller=hex(mem_addr)
                for map in maps:
                    if mem_addr>map['start'] and mem_addr<map['end']:
                       abs_adr=hex(mem_addr-map['start'])
                       pos_caller=subprocess.check_output(['addr2line', '-e', map['symbol'], abs_adr ])
                       break
                print(str(entry['count'])+' allocs at '+ entry['symbol']+':'+pos_caller)
            else:
                print('Malformed line: '+entry['position'])
               
    f.close()

def parse_file(mtrace_file):
    reg_string='^0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+at\s+0x([0-9a-f]+)$'
    re.compile(reg_string)
    print('Parse')
    proc = Popen(['mtrace', mtrace_file], stdout=PIPE)
    skip_line=True
    print('Parse2')
    for line in proc.stdout:
        print(line)
        if skip_line:
            if line.startswith('Memory not freed'):
                skip_line=False
            else:
                continue
        m=re.match(reg_string, line)
        if m != None:
            address = int(m.group(1),16)
            size = int(m.group(2),16)
            caller_address = int(m.group(3),16)
            pos_caller=''
            pos_addr=hex(address)
            for map in maps:
                if address>map['start'] and address<map['end']:
                    abs_adr=hex(address-map['start'])
                    pos_addr=subprocess.check_output(['addr2line', '-e', map['symbol'], abs_adr ])
                    break
            for map in maps:
                if caller_address>map['start'] and caller_address<map['end']:
                    abs_adr=hex(caller_address-map['start'])
                    pos_caller=subprocess.check_output(['addr2line', '-e', map['symbol'], abs_adr ])
                    break
            print( str(size)+' Bytes lost in '+(str(pos_caller)).strip()+' ->'+(str(pos_addr)).strip() )

if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description='Process output of scorep_substrate_mtrace.')
    parser.add_argument('filename',
                        help='file name of the mtrace file')
    
    args = parser.parse_args()
    filename=args.filename;
    
    try:
        subprocess.check_call(['which','addr2line'])
    except err:
        printf('Could not find addr2line')
    
    try:
        subprocess.check_call(['which','mtrace'])
    except err:
        printf('Could not find mtrace')
    
    
    parse_mmap(filename+'.maps')
    
    alloc_stat(filename)
    
    parse_file(filename)