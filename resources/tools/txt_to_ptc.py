#!/bin/python3
import sys       # for command line args
import hashlib   # for md5 in .PTC files

PTC_PRG_TYPE = b"PETC0300RPRG"
PRG_EXTRA_HEADER_SIZE = 0x0c

def as_u32(number, pad=False):
	# also can doe padding to multiple of 4
	bstr = b""
	if (pad):
		pad = (4 - (number % 4) if number % 4 else 0)
	number += pad
	for i in range(0,4):
		bstr += chr(number % 256).encode()
		number >>= 8
	return bstr

def write_program(ptc_file, filename, data):
	# See https://petitcomputer.fandom.com/wiki/PTC_File_Formats for header deatils
	# Header
	ptc_file.write(b"PX01")
	ptc_file.write(as_u32(len(data) + len(PTC_PRG_TYPE) + PRG_EXTRA_HEADER_SIZE, True)) # filesize
	ptc_file.write(b"\x00\x00\x00\x00")
	filename += "\0\0\0\0\0\0\0\0"
	ptc_file.write((filename[:8]).encode()) # always 8 bytes
	
	null_8 = b"\0\0\0\0\0\0\0\0"
	prg_size = as_u32(len(data))
	while len(data) % 4:
		data += b"\0" # pad zeros until multiple of 4
	
	# md5 information
	petit_md5 = hashlib.md5(b"PETITCOM")
	petit_md5.update(PTC_PRG_TYPE)      # PRG type id
	petit_md5.update(null_8)            # package details: can be ignored
	petit_md5.update(prg_size)
	petit_md5.update(data)              # actual program info
	ptc_file.write(petit_md5.digest())  # hash as described on linked page
	ptc_file.write(PTC_PRG_TYPE)        # file type
	# Extra header for PRG
	ptc_file.write(null_8)
	ptc_file.write(prg_size)
	# Contents
	ptc_file.write(data)
	
filename = sys.argv[1] # source
output = sys.argv[2] if len(sys.argv) > 2 else filename.replace("\\","/").split("/")[-1][:8]

# read original file
with open(filename,'rb') as f:
	data = f.read()
# convert and add .PTC to name
with open(filename+".PTC",'wb') as f:
	write_program(f, filename, data)
