import sys       # for command line args
import hashlib   # for md5 in .PTC files

PTC_CHR_SIZE = 256*32  # in bytes
PTC_CHR_TYPE = b"PETC0100RCHR"

NCGR_HEADER_SIZE = 0x10
# NOTE: CHAR != CHR
# CHAR is the magic ID within NCGR files.
# CHR is a filetype in PTC.
CHAR_HEADER_SIZE = 0x20
CHAR_SIZE_OFS = 0x4
CHAR_SIZE_LEN = 0x4

def as_u32(bin_str):
	return bin_str[0] + (bin_str[1] << 8) + (bin_str[2] << 16) + (bin_str[3] << 24)

def write_ptc_header(ptc_file, filename, data):
	# See https://petitcomputer.fandom.com/wiki/PTC_File_Formats for header deatils
	ptc_file.write(b"PX01")             # magic id
	ptc_file.write(b"\x0c\x20\x00\x00") # filesize, same for all CHR files
	ptc_file.write(b"\x03\x00\x00\x00") # ??? multiple of my files have this, but disagrees with linked page?
	ptc_file.write(filename.encode())
	ptc_file.write(b"\x00\x00\x00\x00") # filename padding (assumes filename="XYZ#" format)
	petit_md5 = hashlib.md5(b"PETITCOM")
	petit_md5.update(PTC_CHR_TYPE)
	petit_md5.update(data)
	ptc_file.write(petit_md5.digest())  # hash as described on linked page
	ptc_file.write(PTC_CHR_TYPE)        # file type

def extract_ncgr_to_multiple_ptc(ncgr_file, ptc_filetype):
	"""
	Extracts one NCGR file to multiple PTC files.
	ncgr_file File path to NCGR filename
	ptc_filetype PTC CHR subtype (ex. BGU, SPU, BGD)
	"""
	with open(ncgr_file,"rb") as ncgr:
		std_header = ncgr.read(0x10)  # RGCN (NCGR) block
		ncgr_header = ncgr.read(0x20)  # RAHC (CHAR) block
		ncgr_size = as_u32(ncgr_header[CHAR_SIZE_OFS:CHAR_SIZE_OFS+CHAR_SIZE_LEN]) - CHAR_HEADER_SIZE
		data = ncgr.read(ncgr_size)  # The entire actual data section of the NCGR
		chunks = len(data)//PTC_CHR_SIZE  # Split NCGR into a number PTC files, which are of fixed size
		for i in range(0,chunks):
			with open(ptc_filetype + str(i) + ".PTC", "wb") as ptc_file:
				write_ptc_header(ptc_file, ptc_filetype+str(i), data[i*PTC_CHR_SIZE:(i+1)*PTC_CHR_SIZE])
				ptc_file.write(data[i*PTC_CHR_SIZE:(i+1)*PTC_CHR_SIZE])

ncgr_filename = sys.argv[1]
ptc_type = sys.argv[2]

extract_ncgr_to_multiple_ptc(ncgr_filename, ptc_type)
