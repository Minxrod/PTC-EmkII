#!/bin/python3
import qrcode
import io
import sys
import zlib
from math import ceil

from ptc_file import PTCFile, to_bytes, md5

# shoutouts to this document here for all of the information
# https://gist.github.com/ajc2/25258be3296847bc55cec9e27d13f053

to_convert = sys.argv[1]

ptc = PTCFile(to_convert)

qrs = qrcode.QRCode(
    version=20,
    error_correction=qrcode.constants.ERROR_CORRECT_M,
    border=4,
)

compressed = zlib.compress(ptc.get_internal_file())

data = ptc.filename
data += ptc.type_str[8:]
data += to_bytes(len(compressed))
data += to_bytes(ptc.size)
data += compressed

# print(data)

chunk_size = 666
segment_size = chunk_size - 36

chunks = []
max_qrs = ceil(len(data)/segment_size)
print(max_qrs)
for i in range(1,max_qrs+1):
	chunk = b"PT"
	chunk += to_bytes(i)[0:1]
	chunk += to_bytes(max_qrs)[0:1] # max qrs
	segment = data[(i-1)*segment_size:i*segment_size]
	chunk += md5(segment)
	chunk += md5(data)
	chunk += segment
	chunks.append(chunk)
	
	qrs.clear()
	qrs.add_data(chunk, optimize=0)
	qrs.make(fit=False)
	
	img = qrs.make_image()
	img.show()
	print(chunk)
	
	import time
	time.sleep(30)

#	f = io.StringIO()
#	qrs.print_ascii(out=f)
#	f.seek(0)
#	print(f.read())
#	print(chunk)


