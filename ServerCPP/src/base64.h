
#ifndef _ADCS__BASE64_H_
#define _ADCS__BASE64_H_

#include <string>
using namespace std;

namespace ADCS {

class Base64
{
public:
	static string encode(const char* data);
	static string encode(const string & data);
	static string encode(const unsigned char * data, unsigned int len);
	static string decode(const string & data);
	static int decode(const string & data, unsigned char* dataO, unsigned int bufflen);

	// Returns maximum size of decoded data based on size of Base64 code.
	static unsigned int GetDecodeLength(unsigned int codeLength);
	// Returns maximum length of Base64 code based on size of uncoded data.
	static unsigned int GetBase64Length(unsigned int dataLength);
private:
	static const string Base64Table;
	static const string::size_type DecodeTable[];


};

}

#endif
