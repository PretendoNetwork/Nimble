import sys, os.path

if(len(sys.argv) > 1):
	filename = sys.argv[1]
elif len(sys.argv) == 1:
	if "/" and "." in sys.argv[0]:
		filename = sys.argv[0]
	else:
		filename = raw_input("File name: ")

filename = os.path.split(filename)[1]


var_name = filename.replace(".", "_")

fh = open(filename, "rb") 	# Handle
fc = fh.read()				# Content
fh.seek(0,2)
fl = fh.tell()				# Length
fh.seek(0,0)

nfh = open(var_name + ".h", "w") # New File Handle

newfile = "static const char " + var_name + "[] = {\n\n"

for i in range(0, fl):
	c = ord(fc[i])					# Character as ASCII int
	c = hex(c).replace("0x", "")	# Character as Hexadecimal String

	if len(c) == 1:					# Formatting ...
		c = "0x0" + c
	else:
		c = "0x" + c

	newfile += c + ", "				# Add value to the file

	if ((i+1) % 16) == 0:
		newfile += "\n"				# Newline if there are 16 character on a line

newfile += "\n};\n"	# Add file size
nfh.write(newfile)															# Write data to the new file
fh.close()																	# Close original file handle
nfh.close()																	# Close new file handle