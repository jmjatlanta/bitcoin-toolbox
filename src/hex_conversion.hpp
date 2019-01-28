#pragma once

#include <string>
#include <vector>
#include <utility>
#include <cstdint>

namespace bc_toolbox {
   // constants
   unsigned const char OP_0 = 0x00;
   unsigned const char OP_FALSE = 0x00;
   unsigned const char OP_PUSHDATA1 = 0x4c;
   unsigned const char OP_PUSHDATA2 = 0x4d;
   unsigned const char OP_PUSHDATA4 = 0x4e;
   unsigned const char OP_1 = 0x51;
   unsigned const char OP_TRUE = 0x51;
   unsigned const char OP_2 = 0x52;
   unsigned const char OP_3 = 0x53;
   unsigned const char OP_4 = 0x54;
   unsigned const char OP_5 = 0x55;
   unsigned const char OP_6 = 0x56;
   unsigned const char OP_7 = 0x57;
   unsigned const char OP_8 = 0x58;
   unsigned const char OP_9 = 0x59;
   unsigned const char OP_10 = 0x5a;
   unsigned const char OP_11 = 0x5b;
   unsigned const char OP_12 = 0x5c;
   unsigned const char OP_13 = 0x5d;
   unsigned const char OP_14 = 0x5e;
   unsigned const char OP_15 = 0x5f;
   unsigned const char OP_16 = 0x60;
   unsigned const char OP_1NEGATE = 0x4f;
   // flow control
   unsigned const char OP_NOP = 0x61;
   unsigned const char OP_IF = 0x63;
   unsigned const char OP_NOTIF = 0x64;
   unsigned const char OP_ELSE = 0x67;
   unsigned const char OP_ENDIF = 0x68;
   unsigned const char OP_VERIFY = 0x69;
   unsigned const char OP_RETURN = 0x6a;
   // stack
   unsigned const char OP_TOTALSTACK = 0x6b;
   unsigned const char OP_FROMALTSTACK = 0x6c;
   unsigned const char OP_IFDUP = 0x73; 
   unsigned const char OP_DEPTH = 0x74;
   unsigned const char OP_DROP = 0x75;
   unsigned const char OP_DUP = 0x76;
   unsigned const char OP_NIP = 0x77;
   unsigned const char OP_OVER = 0x78;
   unsigned const char OP_PICK = 0x79;
   unsigned const char OP_ROLL = 0x7a;
   unsigned const char OP_ROT = 0x7b;
   unsigned const char OP_SWAP = 0x7c;
   unsigned const char OP_TUCK = 0x7d;
   unsigned const char OP_2DROP = 0x6d;
   unsigned const char OP_2DUP = 0x6e;
   unsigned const char OP_3DUP = 0x6f;
   unsigned const char OP_2OVER = 0x70;
   unsigned const char OP_2ROT = 0x71;
   unsigned const char OP_2SWAP = 0x72;
   // splice
   unsigned const char OP_CAT = 0x7e; // disabled
   unsigned const char OP_SUBSTR = 0x7f; // disabled
   unsigned const char OP_LEFT = 0x80; // disabled
   unsigned const char OP_RIGHT = 0x81; // disabled
   unsigned const char OP_SIZE = 0x82;
   // bitwise logic
   unsigned const char OP_INVERT = 0x83; // disabled
   unsigned const char OP_AND = 0x84; // disabled
   unsigned const char OP_OR = 0x85; // disabled
   unsigned const char OP_XOR = 0x86; // disabled
   unsigned const char OP_EQUAL = 0x87;
   unsigned const char OP_EQUALVERIFY = 0x88;
   // arithmetic
   unsigned const char OP_1ADD = 0x8b;
   unsigned const char OP_1SUB = 0x8c;
   unsigned const char OP_2MUL = 0x8d; // disabled
   unsigned const char OP_2DIV = 0x8e; // disabled
   unsigned const char OP_NEGATE = 0x8f;
   unsigned const char OP_ABS = 0x90;
   unsigned const char OP_NOT = 0x91;
   unsigned const char OP_0NOTEQUAL = 0x92;
   unsigned const char OP_ADD = 0x93;
   unsigned const char OP_SUB = 0x94;
   unsigned const char OP_MUL = 0x95; // disabled
   unsigned const char OP_DIV = 0x96; // disabled
   unsigned const char OP_MOD = 0x97; // disabled
   unsigned const char OP_LSHIFT = 0x98; // disabled
   unsigned const char OP_RSHIFT = 0x99; // disabled
   unsigned const char OP_BOOLAND = 0x9a;
   unsigned const char OP_BOOLOR = 0x9b;
   unsigned const char OP_NUMEQUAL =0x9c;
   unsigned const char OP_NUMEQUALVERIFY = 0x9d;
   unsigned const char OP_NUMNOTEQUAL = 0x9e;
   unsigned const char OP_LESSTHAN = 0x9f;
   unsigned const char OP_GREATERTHAN = 0xa0;
   unsigned const char OP_LESSTHANOREQUAL = 0xa1;
   unsigned const char OP_GREATERTHANOREQUAL = 0xa2;
   unsigned const char OP_MIN = 0xa3;
   unsigned const char OP_MAX = 0xa4;
   unsigned const char OP_WITHIN = 0xa5;
   // crypto
   unsigned const char OP_RIPEMD160 = 0xa6;
   unsigned const char OP_SHA1 = 0xa7;
   unsigned const char OP_SHA256 = 0xa8;
   unsigned const char OP_HASH160 = 0xa9;
   unsigned const char OP_HASH256 = 0xaa;
   unsigned const char OP_CODESEPARATOR = 0xab;
   unsigned const char OP_CHECKSIG = 0xac;
   unsigned const char OP_CHECKSIGVERIFY = 0xad;
   unsigned const char OP_CHECKMULTISIG = 0xae;
   unsigned const char OP_CHECKMULTISIGVERIFY = 0xaf;
   // locktime
   unsigned const char OP_CHECKLOCKTIMEVERIFY = 0xb1;
   unsigned const char OP_CHECKSEQUENCEVERIFY = 0xb2;
   // pseudo-words
   unsigned const char OP_PUBKEYHASH = 0xfd;
   unsigned const char OP_PUBKEY = 0xfe;
   unsigned const char OP_INVALIDOPCODE = 0xff;
   // reserved words
   unsigned const char OP_RESERVED = 0x50;
   unsigned const char OP_VER = 0x62;
   unsigned const char OP_VERIF = 0x65;
   unsigned const char OP_VERNOTIF = 0x66;
   unsigned const char OP_RESERVED1 = 0x89;
   unsigned const char OP_RESERVED2 = 0x8a;
   unsigned const char OP_NOP1 = 0xb0;
   unsigned const char OP_NOP4 = 0xb3;
   unsigned const char OP_NOP5 = 0xb4;
   unsigned const char OP_NOP6 = 0xb5;
   unsigned const char OP_NOP7 = 0xb6;
   unsigned const char OP_NOP8 = 0xb7;
   unsigned const char OP_NOP9 = 0xb8;
   unsigned const char OP_NOP10 = 0xb9;

   // deal with integers
   std::pair<uint8_t, std::vector<uint8_t> > pack(int64_t incoming);
   // convert opcodes to strings
   std::string opcode_to_string(unsigned const char op_code);
}

