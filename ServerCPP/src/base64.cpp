
#include "base64.h"
#include <string.h>

namespace ADCS {

static const char fillchar = '=';
static const string::size_type np = string::npos;

const string Base64::Base64Table(
  // 0000000000111111111122222222223333333333444444444455555555556666
  // 0123456789012345678901234567890123456789012345678901234567890123
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

// Decode Table gives the index of any valid base64 character in the 
// Base64 table
// 65 == A, 97 == a, 48 == 0, 43 == +, 47 == /

const string::size_type Base64::DecodeTable[] = {
// 0  1  2  3  4  5  6  7  8  9
  np,np,np,np,np,np,np,np,np,np,  // 0 - 9
  np,np,np,np,np,np,np,np,np,np,  //10 -19
  np,np,np,np,np,np,np,np,np,np,  //20 -29
  np,np,np,np,np,np,np,np,np,np,  //30 -39
  np,np,np,62,np,np,np,63,52,53,  //40 -49
  54,55,56,57,58,59,60,61,np,np,  //50 -59
  np,np,np,np,np, 0, 1, 2, 3, 4,  //60 -69
   5, 6, 7, 8, 9,10,11,12,13,14,  //70 -79
  15,16,17,18,19,20,21,22,23,24,  //80 -89
  25,np,np,np,np,np,np,26,27,28,  //90 -99
  29,30,31,32,33,34,35,36,37,38,  //100 -109
  39,40,41,42,43,44,45,46,47,48,  //110 -119
  49,50,51,np,np,np,np,np,np,np,  //120 -129
  np,np,np,np,np,np,np,np,np,np,  //130 -139
  np,np,np,np,np,np,np,np,np,np,  //140 -149
  np,np,np,np,np,np,np,np,np,np,  //150 -159
  np,np,np,np,np,np,np,np,np,np,  //160 -169
  np,np,np,np,np,np,np,np,np,np,  //170 -179
  np,np,np,np,np,np,np,np,np,np,  //180 -189
  np,np,np,np,np,np,np,np,np,np,  //190 -199
  np,np,np,np,np,np,np,np,np,np,  //200 -209
  np,np,np,np,np,np,np,np,np,np,  //210 -219
  np,np,np,np,np,np,np,np,np,np,  //220 -229
  np,np,np,np,np,np,np,np,np,np,  //230 -239
  np,np,np,np,np,np,np,np,np,np,  //240 -249
  np,np,np,np,np,np               //250 -256
};

string Base64::encode(const unsigned char * data, unsigned int len) {
  size_t i;
  char c;
  string ret;

  ret.reserve(GetBase64Length(len));

  for (i = 0; i < len; ++i) {
	c = (data[i] >> 2) & 0x3f;
	ret.append(1, Base64Table[c]);
	c = (data[i] << 4) & 0x3f;
	if (++i < len) {
	  c |= (data[i] >> 4) & 0x0f;
	}

	ret.append(1, Base64Table[c]);
	if (i < len) {
	  c = (data[i] << 2) & 0x3f;
	  if (++i < len) {
		c |= (data[i] >> 6) & 0x03;
	  }
	  ret.append(1, Base64Table[c]);
	} else {
		++i;
		ret.append(1, fillchar);
	}

	if (i < len) {
		c = data[i] & 0x3f;
		ret.append(1, Base64Table[c]);
	} else {
		ret.append(1, fillchar);
	}
  }

  return ret;
}

string Base64::encode(const string& data)
{

	return encode(data.c_str());
}

 string Base64::encode(const char* data)
 {
	//convert to binary data
	unsigned int iDataLen = (unsigned int)(strlen(data) * sizeof(char));
	unsigned char* buffer = new unsigned char[iDataLen+1];
	memset(buffer, 0, iDataLen+1);
	memcpy(buffer, data, iDataLen);

	string sRet = encode(buffer, iDataLen);

    delete[] buffer;

	return sRet;
 }
    

 int Base64::decode(const string & data, unsigned char* dataO, unsigned int bufflen)
 {
	char c;
	char c1;
    string::size_type i;
	string::size_type len = data.length();

	unsigned int iOffset = 0;
	unsigned int iDataLen = GetDecodeLength((unsigned int)data.length());
	//size_t iBufferLen = iDataLen * sizeof(TCHAR);

	if( dataO == NULL || bufflen < iDataLen ) return 0;

	for (i = 0; i < len; ++i) 
	{
		do
		{
			c = static_cast<unsigned char>(DecodeTable[static_cast<unsigned char>(data[i])]);
		} while (c == np && ++i < len);

		++i;

		do {
			c1 = static_cast<unsigned char>(DecodeTable[static_cast<unsigned char>(data[i])]);
		} while (c == np && ++i < len);

		c = (c << 2) | ((c1 >> 4) & 0x3);
		dataO[iOffset] = static_cast<unsigned char>(c);
		iOffset++;
		if (++i < len) 
		{
			c = data[i];
			if (fillchar == c) { break; }

			do 
			{
				c = static_cast<unsigned char>(DecodeTable[static_cast<unsigned char>(data[i])]);
			} while (c == np && ++i < len);
			c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
			dataO[iOffset] = static_cast<unsigned char>(c1);
			iOffset++;
		}

		if (++i < len)
		{
			c1 = data[i];
			if (fillchar == c1) { break; }

			do
			{
				c1 = static_cast<unsigned char>(DecodeTable[static_cast<unsigned char>(data[i])]);
			} while (c == np && ++i < len);

			c = ((c << 6) & 0xc0) | c1;
			dataO[iOffset] = static_cast<unsigned char>(c);
			iOffset++;
		}
	}

	return iDataLen;
 }

string Base64::decode(const string& data)
{
	string ret;

	unsigned int iDataLen = GetDecodeLength((unsigned int)data.length());
	unsigned int iBufferLen = iDataLen  * sizeof(char);
	unsigned char* buffer = new unsigned char[iBufferLen];
	memset(buffer, 0, iBufferLen);

	decode(data, buffer, iBufferLen);

	char* tOut = new char[iDataLen];
	memcpy(tOut, buffer, iDataLen * sizeof(char));

	ret = tOut;

    delete[] buffer;
    delete[] tOut;


	return ret;
}

// Returns maximum size of decoded data based on size of Base64 code.
unsigned int Base64::GetDecodeLength(unsigned int codeLength)
{
	return codeLength - codeLength / 4;
}

// Returns maximum length of Base64 code based on size of uncoded data.
unsigned int Base64::GetBase64Length(unsigned int dataLength)
{
	unsigned int len = dataLength + dataLength / 3 + (int)( dataLength % 3 != 0 );

	// output code size must be multiple of 4 bytes
	if( len % 4 )
		len += 4 - len % 4;

	return len;
}


}

