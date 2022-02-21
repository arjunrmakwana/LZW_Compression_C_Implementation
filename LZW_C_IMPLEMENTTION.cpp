
// LZW_C_IMPLEMENTTION.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
	This program reads a .txt file and compresses it using the LZW algorithm. 
	The compression result is stored in a buffer where it is packed at arbitrary 
	bit length using the to_code_len() function. 
	encoding() is the main compression function.
*/



#include <iostream>
#include <stdlib.h>
#include <unordered_map>
#include <math.h>
#include <string.h>
#include <stdint.h>
#define table_tt = 512
#define CODE_LENGTH 13
#define MAX_SIZE 3000000
uint16_t to_cl_written; // = 0;
uint16_t to_cl_to_be_written; // = CODE_LENGTH;
uint16_t to_cl_capacity; // = 8;
uint16_t to_cl_idx;

void encoding(unsigned char* ip, int len, unsigned char* op);

void to_code_len(uint16_t num, unsigned char* op) {

	to_cl_to_be_written = CODE_LENGTH;
	unsigned char temp;

	while (to_cl_to_be_written != 0) {

		if (to_cl_to_be_written >= 8) {

			temp = num >> (to_cl_to_be_written - to_cl_capacity);
			op[to_cl_idx] |= temp;

			to_cl_written = to_cl_capacity;
			to_cl_to_be_written = to_cl_to_be_written - to_cl_written;
			to_cl_capacity = to_cl_capacity - to_cl_written;

			if (to_cl_capacity <= 0) { to_cl_idx++; to_cl_capacity = 8; }

		}

		if (to_cl_to_be_written < 8) {

			temp = num << (8 - to_cl_to_be_written);
			op[to_cl_idx] |= temp;

			to_cl_written = to_cl_to_be_written;
			to_cl_capacity = 8 - to_cl_written;
			to_cl_to_be_written = 0;

			if (to_cl_capacity <= 0) { to_cl_idx++; to_cl_capacity = 8; }

		}

	}

}

void top_function(unsigned char *buff, int buff_len) {
	
	uint32_t size_required; // = uint32_t(ceil((CODE_LENGTH * len) / 8)) + 1;

	if (CODE_LENGTH * buff_len % 8 == 0) {
		size_required = CODE_LENGTH * buff_len / 8;
	}
	else {
		size_required = (CODE_LENGTH * buff_len / 8) + 1;
	}

	unsigned char* op = (unsigned char*)calloc(size_required, sizeof(unsigned char));
	std::cout << "size_reqired = " << size_required << std::endl;
	encoding(buff, buff_len, op);

}

void string_copy(unsigned char* op, unsigned char * ip, int len) {
	for (int i = 0; i < len;i++) {
		op[i] = ip[i];
	}
}

void string_cat(unsigned char* ip, unsigned char* op, int start_idx, int len) {
	for (int i = start_idx; i < start_idx + len; i++) {
		ip[i] = op[i];
	}
}

int in_the_table(unsigned char table[8192][257], int table_len, unsigned char* ip, int ip_len) {

	bool keep_going = true;

	for (int j = 0; j < table_len; j++) { 
		//dont need to go through the entire table. Just go through the number of time the table has been written.

		if (table[j][0] == ip_len) {
			//start string comparison only if the string length matches.
			int col_idx = 0;

			while ((col_idx < ip_len) && keep_going) {
				if (table[j][col_idx + 1] != ip[col_idx]) { 
					keep_going = false; 
					//if a single character does not mach, no need to check for the rest.
				}
				col_idx++;
						
			}

			if (keep_going) {
				return j;
				// found the string. Return it.
			}
			//else { keep_going = true; }
			keep_going = true;
			// did not find the string at the current index. Look for it in the next index. Continue this till the number of entries in the table.
		}
	}
	return (-1);
	//return -1 if string not in the table.
}


void encoding(unsigned char* ip, int len, unsigned char *op) {
    
	/*LZW encoding where I am using a 8912 by 246 byte table to store the data.
	The table allows at max to store a string of 256 bytes i.e. a string of 256 characters.
	We can store at max 8192 strings.
	The first byte of each of the 8192 string entries corresponds to the length of the string stored at the location.
	While this is not needed for the algorithm, it makes the process of comparing the incoming string with that in the table a bit faster.
	You dont need to check all the 256 string elements; you first check for the string length and only if it matches, we start comparing the strings.
	Also if we come accross any discrepencies in the string we have and the string we encounter in the table, we immediately move on to the next entry and dont check the rest of the characters.
	*/

	to_cl_written = 0;
	//to_cl_to_be_written = CODE_LENGTH;
	to_cl_capacity = 8;
	to_cl_idx = 0;
	
	unsigned char lzw_table[8192][257] = {0};
	int table_written = 0;
    //int code = 256;

	int how_much_written = 0;


    std::cout << "initializing lzw table." << std::endl;

    for (int i = 0; i < 256; i++) {
        //unsigned char ch = char(i);
        lzw_table[i][1] = char(i);
        lzw_table[i][0] = 1;
 
    }
	table_written = 256;
    std::cout << "Encoding." << std::endl;

	unsigned char p[256] = { 0 };
	unsigned char c;
	unsigned char p_plus_c[256] = {0};

	uint16_t p_len=0, p_plus_c_len=0;

	p[0] = ip[0];
	p_len++;
	int i = 0;
	
	while (i < len) {
		
		string_copy(p_plus_c, p, p_len);
		p_plus_c_len = p_len;

		if ( (i != len - 1) && (p_plus_c_len < 256)) {
			c = ip[i + 1];
			p_plus_c[p_plus_c_len] = c;
			p_plus_c_len++;
		}

		

		

		int result = in_the_table(lzw_table, table_written, p_plus_c, p_plus_c_len);
		std::cout << "result = " << result << std::endl;
		std::cout << "p is : " << p << " which is at idx: " << result << " in the lzw table." << std::endl;
		if (result != -1) {
			string_copy(p, p_plus_c, p_plus_c_len);
			p_len = p_plus_c_len;
		}

		else{

			uint16_t p_result = in_the_table(lzw_table, table_written, p, p_len);
			std::cout << "p is : " << p << " which is at idx: " << p_result << " in the lzw table." << std::endl;
			to_code_len(p_result, op);
			how_much_written++;
			if (table_written < 8192) {
				for (int idx = 0; idx < p_plus_c_len; idx++) {
					lzw_table[table_written][idx + 1] = p_plus_c[idx];
				}
				lzw_table[table_written][0] = p_plus_c_len;

				table_written++;
			}
			
			//how_much_written++;
			
			//code++;

			//string_copy(p, c, 1);
			for (int idx = 0; idx <= p_len;idx++) {
				p[idx] = 0X00;
			}
			p[0] = c;
			p_len = 1;
		}
		c = 0x00;
		i++;
	}
	//uint16_t p_result = in_the_table(lzw_table, table_written, p, p_len);
	//to_code_len(p_result, op);

}




void test_lzw(const char* file)
{
	FILE* fp = fopen(file, "r");
	if (fp == NULL) {
		perror("fopen error");
		return;
	}

	fseek(fp, 0, SEEK_END); // seek to end of file
	int file_size = ftell(fp); // get current file pointer
	std::cout << "file size: " << file_size << std::endl;
	fseek(fp, 0, SEEK_SET); // seek back to beginning of file

	unsigned char* buff = (unsigned char*)malloc((sizeof(unsigned char) * file_size));
	if (buff == NULL)
	{
		perror("not enough space");
		fclose(fp);
		return;
	}

	std::cout << buff;

	int bytes_read = fread(&buff[0], sizeof(unsigned char), file_size, fp);

	top_function(buff, file_size);

	free(buff);
	return;
}
 

int main() {
	test_lzw("LittlePrince.txt");
	
	unsigned char buff[] = "thisisthe";
	//std::cout << "len of the string is :" << strlen((char*)buff) << std::endl;
	//top_function(buff, strlen((char*)buff));
}

