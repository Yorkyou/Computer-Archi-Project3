//
//  register.hpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/16.
//  Copyright © 2017年 York You. All rights reserved.
//

#ifndef register_hpp
#define register_hpp
#include <bitset>
#include <sstream>
using namespace std;

class Reg{
    friend class Instruction;
protected:
    bitset<32>reg[35];//registers:HI(32),LO(33),PC(34),SP(29),RA(31)each view as 32bits
    bool PCisadjusted;
    bool halt;
    enum {pos,neg,zero};
    bool ismult;
public:
    Reg(bitset<32> SP,bitset<32> PC);
    long long BitsetToSigned32(bitset<32>);//there is no to_long() function in bitset.
    long BitsetToSigned16(bitset<16>);
    void PCmoveForward();
    //R-type Inst.
    void add(long rs,long rt,long rd,int cycle);
    void addu(long rs,long rt,long rd,int cycle);
    void sub(long rs,long rt,long rd,int cycle);
    void And(long rs,long rt,long rd,int cycle);
    void Or(long rs,long rt,long rd,int cycle);
    void Xor(long rs,long rt,long rd,int cycle);
    void Nor(long rs,long rt,long rd,int cycle);
    void Nand(long rs,long rt,long rd,int cycle);
    void slt(long rs,long rt,long rd,int cycle);
    void sll(long rd,long rt,long C,int cycle);
    void srl(long rd,long rt,long C,int cycle);
    void sra(long rd,long rt,long C,int cycle);
    void jr(long rs,int cycle);
    void mult(long rs,long rt,int cycle);
    void multu(long rs,long rt,int cycle);
    void mfhi(long rd,int cycle);
    void mflo(long rd,int cycle);
    //I-type inst.
    void addi(long rt,long rs,long i,int cycle);
    void addiu(long rt,long rs,long i,int cycle);
    bitset<32> lw(long rt,long i,long rs,int cycle);
    bitset<32> lh(long rt,long i,long rs,int cycle);
    bitset<32> lhu(long rt,long i,long rs,int cycle);
    bitset<32> lb(long rt,long i,long rs,int cycle);
    bitset<32> lbu(long rt,long i,long rs,int cycle);
    bitset<32> sw(long rt,long i,long rs,int cycle);
    bitset<32> sh(long rt,long i,long rs,int cycle);
    bitset<32> sb(long rt,long i,long rs,int cycle);
    void lui(long rt,bitset<16> i,int cycle);
    void Andi(long rt,long rs,long i,int cycle);
    void Ori(long rt,long rs,long i,int cycle);
    void Nori(long rt,long rs,long i,int cycle);
    void slti(long rt,long rs,long i,int cycle);
    void beq(long rs,long rt,long i,int cycle);
    void bne(long rs,long rt,long i,int cycle);
    void bgtz(long rs,long i,int cycle);
    //J-type inst.
    void j(bitset<26> adr,int cycle);
    void jal(bitset<26> adr,int cycle);
    //halt
};

#endif /* register_h */
