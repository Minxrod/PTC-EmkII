import sys       # for command line args
import hashlib   # for md5 in .PTC files

PTC_COL_SIZE = 256*2  # in bytes
PTC_COL_TYPE = b"PETC0100RCOL"

NCLR_HEADER_SIZE = 0x10
PLTT_HEADER_SIZE = 0x18
PLTT_SIZE_OFS = 0x4
PLTT_SIZE_LEN = 0x4

def as_u32(bin_str):
	return bin_str[0] + (bin_str[1] << 8) + (bin_str[2] << 16) + (bin_str[3] << 24)

def write_ptc_header(ptc_file, filename, data):
	# See https://petitcomputer.fandom.com/wiki/PTC_File_Formats for header deatils
	ptc_file.write(b"PX01")             # magic id
	ptc_file.write(b"\x0c\x02\x00\x00") # filesize, same for all COL files
	ptc_file.write(b"\x05\x00\x00\x00") # ??? multiple of my files have this, but disagrees with linked page?
	ptc_file.write(filename.encode())
	ptc_file.write(b"\x00\x00\x00\x00") # filename padding (assumes filename="XYZ#" format)
	petit_md5 = hashlib.md5(b"PETITCOM")
	petit_md5.update(PTC_COL_TYPE)
	petit_md5.update(data)
	ptc_file.write(petit_md5.digest())  # hash as described on linked page
	ptc_file.write(PTC_COL_TYPE)        # file type

def extract_nclr_to_ptc(nclr_file, ptc_filename):
	"""
	Extracts one NCLR file to one PTC files.
	nclr_file File path to NCLR
	ptc_filetype PTC name (ex COL0, COL1)
	"""
	with open(nclr_file,"rb") as nclr:
		std_header = nclr.read(NCLR_HEADER_SIZE)  # RLCN (NCLR) block
		nclr_header = nclr.read(PLTT_HEADER_SIZE)  # TTLP (PLTT) block
		# nclr_size = as_u32(nclr_header[PLTT_SIZE_OFS:PLTT_SIZE_OFS+PLTT_SIZE_LEN]) - PLTT_HEADER_SIZE
		# nclr_size can be replaced by PTC_COL_SIZE because it is the same value
		data = nclr.read(PTC_COL_SIZE)  # The entire actual data section of the NCGR
		with open(ptc_filename + ".PTC", "wb") as ptc_file:
			write_ptc_header(ptc_file, ptc_filename, data)
			ptc_file.write(data)

nclr_filename = sys.argv[1]
ptc_filename = sys.argv[2]

extract_nclr_to_ptc(nclr_filename, ptc_filename)
