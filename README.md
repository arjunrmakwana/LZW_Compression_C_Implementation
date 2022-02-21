# LZW_Compression_C_Implementation

This is a C langauage implemenatation of the LZW compression algorithm.
It uses a table of CODE_LENGTH(here 8192) by 257 bytes to store the required information.

8192 limits how many entries we can store in the table while 257 bytes limit the maximum charaters per stored string to 256.
I am using 1 byte of the 257 to store the information about how long the corresponding string is. Since the strings can 
be no longer than 256, we need only 1 byte to represent the data about the string length. 

Storing this data about the length of the string stored reduces the time it takes to search for a string in the table.
The first byte of each entry of the table is the length information. So if the length of the current string does not
match with that of the string stored at the location, then we don't even need to compare the strings. 

This C implementation of LZW compression algorithm was written so that it can be deployed on FPGAs. 
While an unordered_map<std::string, int> using the STL library in C++ makes LZW implementation super easy 
and fast, dynamic memory allocation is not possible on FPGAs and so one has to resort to having a purely
C implemenation.


