import hashlib

PTC_PX01 = 0x0
PTC_SIZE_AFTER_MD5 = 0x4
PTC_TYPE = 0x8
PTC_FILENAME = 0xc
PTC_MD5 = 0x14
PTC_TYPE_STR = 0x24
PTC_DATA = 0x30

PTC_PRG_PACKAGE_HIGH = 0x30
PTC_PRG_PACKAGE = 0x34
PTC_PRG_SIZE = 0x38
PTC_PRG_DATA = 0x3c

PX01 = b"PX01"
MD5_PREFIX = b"PETITCOM"

PRG_TYPE = b"PETC0300RPRG"
MEM_TYPE = b"PETC0200RMEM"
GRP_TYPE = b"PETC0100RGRP"
CHR_TYPE = b"PETC0100RCHR"
SCR_TYPE = b"PETC0100RSCR"
COL_TYPE = b"PETC0100RCOL"

def to_number(bin_str):
	return int.from_bytes(bin_str, byteorder="little")

def to_bytes(number):
	return number.to_bytes(4, byteorder="little")

def ptc_md5(data):
	return hashlib.md5(MD5_PREFIX + data).digest()

def md5(data):
	return hashlib.md5(data).digest()


class PTCFile:
	
	def __init__(self, filename):
		""" 
		Note: Expects SD file 
		TODO: Accept internal formats, QR codes?
		"""
		with open(filename,"rb") as f:
			self._sd_data = f.read()
			
		self.size = to_number(self._sd_data[PTC_SIZE_AFTER_MD5:PTC_SIZE_AFTER_MD5+4])
		self.type_id = to_number(self._sd_data[PTC_TYPE:PTC_TYPE+4])
		self.filename = self._sd_data[PTC_FILENAME:PTC_FILENAME+8]
		self.md5 = self._sd_data[PTC_MD5:PTC_MD5+16]
		self.type_str = self._sd_data[PTC_TYPE_STR:PTC_TYPE_STR+12]
		if (self.type_id): # Other resources
			self.data = self._sd_data[PTC_DATA:]
		else: # PRG type
			self.package = self._sd_data[PTC_PRG_PACKAGE_HIGH:PTC_PRG_PACKAGE_HIGH+8]
			self.data = self._sd_data[PTC_DATA:]
	   
	def get_internal_size(self):
		return self.size

	def get_internal_file(self):
		return self.type_str + self.data
		
	def get_internal_name(self):
		return self.filename

	def get_internal_type_id(self):
		return to_bytes(self.type_id)
