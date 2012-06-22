/*
 * CCSDSSpacePacketPrimaryHeader.hh
 *
 *  Created on: Jun 9, 2011
 *      Author: yuasa
 */

#ifndef CCSDSSPACEPACKETPRIMARYHEADER_HH_
#define CCSDSSPACEPACKETPRIMARYHEADER_HH_

#include <bitset>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>

class CCSDSSpacePacketPacketVersionNumber {
public:
	enum {
		Version1 = 0x000, //000b
		Undefined = 0xffff
	};
};

class CCSDSSpacePacketPacketType {
public:
	enum {
		TelemetryPacket = 0x00, //0b
		CommandPacket = 0x01, //1b
		Undefined = 0xffff

	};
};

class CCSDSSpacePacketSecondaryHeaderFlag {
public:
	enum {
		NotPresent = 0x00, //0b
		Present = 0x01, //1b
		Undefined = 0xffff
	};
};

class CCSDSSpacePacketSequenceFlag {
public:
	enum {
		ContinuationSegument = 0x00, //00b
		TheFirstSegment = 0x01, //01b
		TheLastSegment = 0x02, //10b
		UnsegmentedADU = 0x03, //11b
		Undefined = 0xffff
	};
};

class CCSDSSpacePacketPrimaryHeader {
private:
	std::bitset<3> packetVersionNum;
	std::bitset<1> packetType;
	std::bitset<1> secondaryHeaderFlag;
	std::bitset<11> apid;
	std::bitset<2> sequenceFlag;
	std::bitset<14> sequenceCount;
	std::bitset<16> packetDataLength;

public:
	static const unsigned int PrimaryHeaderLength = 6;
	static const unsigned int MaximumLengthOfDataFieldOfTCPacket = 1010;
	static const unsigned int MaximumLengthOfDataFieldOfTMPacketWithoutADUChannel = 1012;
	static const unsigned int MaximumLengthOfDataFieldOfTMPacketWithADUChannel = 1009;


public:
	CCSDSSpacePacketPrimaryHeader() {
		this->setPacketVersionNum(CCSDSSpacePacketPacketVersionNumber::Version1);
	}

public:
	std::vector<unsigned char> getAsByteVector() {
		std::vector<unsigned char> result;
		std::string str = packetVersionNum.to_string() + packetType.to_string() + secondaryHeaderFlag.to_string()
				+ apid.to_string() + sequenceFlag.to_string() + sequenceCount.to_string();
		unsigned int length = packetDataLength.to_ulong();
		std::bitset<32> bits;
		for (unsigned int i = 0; i < 32; i++) {
			if (str[i] == '0') {
				bits.set(32 - i - 1, 0);
			} else {
				bits.set(32 - i - 1, 1);
			}
		}
		result.push_back((unsigned char) (bits >> 24).to_ulong());
		result.push_back((unsigned char) ((bits << 8) >> 24).to_ulong());
		result.push_back((unsigned char) ((bits << 16) >> 24).to_ulong());
		result.push_back((unsigned char) ((bits << 24) >> 24).to_ulong());
		result.push_back((unsigned char) (length / 0x100));
		result.push_back((unsigned char) (length % 0x100));
		return result;
	}

public:
	void interpret(unsigned char* data) {
		using namespace std;
		packetVersionNum = bitset<3> ((data[0] & 0xe0) >> 5 /* 1110 0000 */);
		packetType = bitset<1> ((data[0] & 0x10) >> 4 /* 0001 0000 */);
		secondaryHeaderFlag = bitset<1> ((data[0] & 0x08) >> 3 /* 0000 1000 */);
		bitset<3> apid_msb3bits((data[0] & 0x07) >> 3);
		bitset<8> apid_lsb8bits(data[1]);
		for (unsigned int i = 0; i < 3; i++) {
			apid.set(i + 8, apid_msb3bits[i]);
		}
		for (unsigned int i = 0; i < 8; i++) {
			apid.set(i, apid_lsb8bits[i]);
		}
		cout << "#data[1]=" << (uint32_t) data[1] << endl;
		cout << "#apid_lsb8bits=" << apid_lsb8bits.to_string() << endl;
		cout << "#apid         =" << apid.to_string() << endl;
		cout << "#apid=" << (uint32_t) apid.to_ulong() << endl;
		sequenceFlag = bitset<2> ((data[2] & 0xc0) >> 6 /* 1100 0000 */);
		bitset<6> sequenceCount_msb6bits(data[2] & 0x3F/* 0011 1111 */);
		bitset<6> sequenceCount_lsb8bits(data[3]);
		for (unsigned int i = 0; i < 6; i++) {
			sequenceCount.set(i + 8, sequenceCount_msb6bits[i]);
		}
		for (unsigned int i = 0; i < 8; i++) {
			sequenceCount.set(i, sequenceCount_lsb8bits[i]);
		}
		packetDataLength = bitset<16> (data[4] * 0x100 + data[5]);
	}

public:
	std::bitset<11> getAPID() const {
		return apid;
	}

	unsigned int getAPIDAsInteger() const {
		return apid.to_ulong();
	}

	unsigned int getPacketDataLength() const {
		return packetDataLength.to_ulong();
	}

	std::bitset<1> getPacketType() const {
		return packetType;
	}

	std::bitset<3> getPacketVersionNum() const {
		return packetVersionNum;
	}

	std::bitset<1> getSecondaryHeaderFlag() const {
		return secondaryHeaderFlag;
	}

	std::bitset<14> getSequenceCount() const {
		return sequenceCount;
	}

	std::bitset<2> getSequenceFlag() const {
		return sequenceFlag;
	}

	void setAPID(unsigned int apid) {
		this->apid = std::bitset<11>(apid);
	}

	void setPacketDataLength(std::bitset<16> packetDataLength) {
		this->packetDataLength = packetDataLength;
	}

	void setPacketDataLength(unsigned int packetDataLength) {
		this->packetDataLength = std::bitset<16>(packetDataLength);
	}

	void setPacketType(unsigned int packetType) {
		this->packetType = std::bitset<1>(packetType);
	}

private:
	void setPacketVersionNum(std::bitset<3> packetVersionNum) {
		this->packetVersionNum = packetVersionNum;
	}

	void setPacketVersionNum(unsigned int packetVersionNum) {
		this->packetVersionNum = std::bitset<3>(packetVersionNum);
	}

public:
	void setSecondaryHeaderFlag(std::bitset<1> secondaryHeaderFlag) {
		this->secondaryHeaderFlag = secondaryHeaderFlag;
	}

	void setSecondaryHeaderFlag(unsigned int secondaryHeaderFlag) {
		this->secondaryHeaderFlag = std::bitset<1>(secondaryHeaderFlag);
	}

	void setSequenceCount(std::bitset<14> sequenceCount) {
		this->sequenceCount = sequenceCount;
	}

	void setSequenceCount(unsigned int sequenceCount) {
		this->sequenceCount = std::bitset<14>(sequenceCount);
	}

	void setSequenceFlag(std::bitset<2> sequentialFlags) {
		this->sequenceFlag = sequentialFlags;
	}

	void setSequenceFlag(unsigned int sequentialFlags) {
		this->sequenceFlag = std::bitset<2>(sequentialFlags);
	}

public:
	virtual std::string toString() {
		using namespace std;
		std::stringstream ss;
		ss << "PrimaryHeader" << endl;
		ss << "PacketVersionNum    : " << packetVersionNum.to_string() << endl;
		ss << "PacketType          : " << packetType.to_string() << endl;
		ss << "SecondaryHeaderFlag : " << secondaryHeaderFlag.to_string() << endl;
		ss << "APID                : " << apid.to_ulong();
		ss << " (0x" << hex << setw(2) << setfill('0') << right << apid.to_ulong() << ")" << left << endl;
		ss << "SequenceFlag        : " << sequenceFlag.to_string();
		switch (sequenceFlag.to_ulong()) {
		case 0:
			ss << " (Continuation segment of user data)" << endl;
			break;
		case 1:
			ss << " (First segment of user data)" << endl;
			break;
		case 2:
			ss << " (Last segment of user data)" << endl;
			break;
		case 3:
			ss << " (Unsegmented user data)" << endl;
			break;
		}
		ss << "SequenceCount       : " << sequenceCount.to_ulong() << endl;
		ss << "PacketDataLength    : " << packetDataLength.to_ulong();
		ss << " (User data field has " << dec <<packetDataLength.to_ulong()+1 << " bytes)" << endl;
		return ss.str();
	}
};

#endif /* CCSDSSPACEPACKETPRIMARYHEADER_HH_ */
