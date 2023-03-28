#include <iostream>
#include <bitset>
#include <string>

#define LengthOfA 93
#define LengthOfB 84
#define LengthOfC 111
#define AFeedForwardBit 65
#define BFeedForwardBit 68
#define CFeedForwardBit 65
#define AFeedBackBit 68
#define BFeedBackBit 77
#define CFeedBackBit 86
#define Byte 8

typedef unsigned short ushort;

using namespace std;

template <ushort degree>
class LFSR
{
	ushort size;
	bitset<degree> reg;
	ushort FFBit;
	ushort FBBit;

public:
	LFSR(ushort FFBit, ushort FBBit) :size(degree), FFBit(FFBit), FBBit(FBBit) {}

	LFSR(bitset<degree> initialReg, ushort FFBit, ushort FBBit) :size(degree), FFBit(FFBit), FBBit(FBBit) {
		reg = initialReg;
	}

	void reset(bitset<degree> initialReg) {
		reg = initialReg;
	}

	bool output() {
		return reg[size - 1] ^ (reg[size - 2] && reg[size - 3]) ^ reg[FFBit];
	}

	void shift(bool input) {
		// performing left shift of 1 instead of right shift because bits are mapped in reverse order
		reg <<= 1;
		reg[0] = reg[FBBit] ^ input;
	}

	typename bitset<degree>::reference operator[] (int i) {
		try {
			if (i < size && i > 0) 
				return reg[i];
			throw exception();
		}
		catch (...) {
			bitset<degree> temp;
			return temp[0];
		}
	}

	void printBits() {
		cout << reg << endl;
	}
};

class Trivium
{
	string key;
	string iv;
	LFSR<LengthOfA> A;
	LFSR<LengthOfB> B;
	LFSR<LengthOfC> C;

	void reset(string iv) {
		A.reset(bitset<LengthOfA>(iv));
		B.reset(bitset<LengthOfB>(key));
		C.reset(bitset<LengthOfC>(0));

		for (int i = LengthOfC - 3; i < LengthOfC; i++) // setting last three bits of C to 1
			C[i] = 1;

		for (int i = 0; i < 1152; i++)
			cycle();
	}

	bool cycle() {
		bool aOut = A.output();
		bool bOut = B.output();
		bool cOut = C.output();

		A.shift(cOut);
		B.shift(aOut);
		C.shift(bOut);

		return aOut ^ bOut ^ cOut;
	}
public:
	Trivium(string key) :key(key), A(AFeedForwardBit, AFeedBackBit), 
		B(BFeedForwardBit, BFeedBackBit), C(CFeedForwardBit, CFeedBackBit) {}

	string encrypt_decrypt(string iv, string input) {
		reset(iv);

		string out = "";

		for (int i = 0; i < input.length(); i++)
			out += to_string((input[i] - '0') ^ cycle());

		return out;
	}
};

string stringToBits(string input) {
	string out = "";
	for (int i = 0; i < input.length(); i++) {
		char ch = input[i];
		string sub = "";
		for (int j = 0; j < Byte; j++)
			// picking up bits of string in reverse order
			sub += to_string(((ch << j) & 0b10000000) >> Byte - 1);
		out += sub;
	}
	return out;
}

string bitsToString(string input) {
	string out = "";
	for (int i = 0; i < input.length(); i += Byte) {
		bitset<Byte> bits(input.substr(i, i + Byte));
		out += char(bits.to_ulong());
	}
	return out;
}

int main() {
	string key = "1235467890";
	string iv = "strategies";
	Trivium trivium(stringToBits(key));

	string ct = trivium.encrypt_decrypt(stringToBits(iv), stringToBits("I am Plaintext."));
	string pt = trivium.encrypt_decrypt(stringToBits(iv), ct);
	cout << "KEY: " << key << endl
		<< "IV: " << iv << endl
		<< "Cipher Text: " << bitsToString(ct) << endl 
		<< "Plain Text: " << bitsToString(pt) << endl;
}
