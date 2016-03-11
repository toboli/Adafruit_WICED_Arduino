import binascii

# Change to match your sketch
TEXT1 = "Adafruit"
TEXT2 = " Industries"

print "CRC32 1 piece :",
print binascii.crc32(TEXT1 + TEXT2) & 0xffffffff

# Or, in two pieces:
crc = binascii.crc32(TEXT1)
crc = binascii.crc32(TEXT2, crc)
print "CRC32 2 piece :",
print crc  & 0xffffffff