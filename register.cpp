//
//  reg.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.
//
#include <iostream>
#include <map>
#include <math.h>
#include "register.hpp"
namespace std{
    template < typename T > std::string to_string(const T&n){
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}
extern  std::map< int,bitset<32> > D_disk;
string BinaryToHex(bitset<32>a){
    string r="";
    bitset<4>data;
    for (int i = 0; i < 8; i++) {//3 2 1 0?
        data[0] = a[i*4];
        data[1] = a[i*4+1];
        data[2] = a[i*4+2];
        data[3] = a[i*4+3];
        if (data.to_ulong() > 9) {
            char temp = char(data.to_ulong()-10+'A');
            r.push_back(temp);
        }
        else r+= to_string(data.to_ulong());
    }
    reverse(r.begin(), r.end());
    return "0x"+r;
}
bitset<64> operator+(bitset<64>r,bitset<64>temp){
    bool carry = false;
    bitset<64> result;
    for (int i = 0; i < 64; i++) {
        result[i] = r[i]^temp[i]^carry;
        carry = (r[i]&temp[i]) | (r[i]&carry) | (temp[i]&carry);
    }
    return result;
}
bitset<32> operator+(bitset<32>r,bitset<32>temp){
    bool carry = false;
    bitset<32> result;
    for (int i = 0; i < 32; i++) {
        result[i] = r[i]^temp[i]^carry;
        carry = (r[i]&temp[i]) | (r[i]&carry) | (temp[i]&carry);
    }
    return result;
}
Reg::Reg(bitset<32> SP,bitset<32> PC){
    std::fill(reg, reg+34, 0);
    reg[29] = SP;
    reg[34] = PC;
    ismult = false;
    PCisadjusted = false;
    halt = false;
}
long long Reg::BitsetToSigned32(bitset<32>data){
    bitset<31>temp;
    for (int i = 0; i < 31; i++) {
        temp[i] = data[i];
    }
    return temp.to_ulong()-data[31]*pow(2, 31);
}
long Reg::BitsetToSigned16(bitset<16>data){
    bitset<15>temp;
    for (int i = 0; i < 15; i++) {
        temp[i] = data[i];
    }
    return temp.to_ulong()-data[15]*pow(2, 15);
}
void Reg::PCmoveForward(){
    this->reg[34] = this->reg[34]+bitset<32>(4);
}
//operations define below
//error def :
//(1)write$0 (2)Number overflow (3)Overwrite HI-LO registers
//(4)Memory address overflow (5)Data misaligned

//signed,(1)(2)
void Reg::add(long rs,long rt,long rd,int cycle){
    if (rd==0) {
        
    }//still need to operate below to know if errors occur.
    int rs_is_pos=zero,rt_is_pos=zero,rd_is_pos=zero;
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos=pos;
    else if (BitsetToSigned32(this->reg[rs])< 0)rs_is_pos=neg;
    if(BitsetToSigned32(this->reg[rt])>0)rt_is_pos=pos;
    else if(BitsetToSigned32(this->reg[rt])<0)rt_is_pos=neg;
    if (rd!=0){
        this->reg[rd] = this->reg[rs]+this->reg[rt];
        if(BitsetToSigned32(this->reg[rd])>0)rd_is_pos = pos;
        else if(BitsetToSigned32(this->reg[rd])<0)rd_is_pos = neg;
        if (rt_is_pos==rs_is_pos &&rs_is_pos!=rd_is_pos) {//means overflow
            //print out sth and cont. the program.
            
        }
    }
    else
    {
        bitset<32>temp = this->reg[rs]+this->reg[rt];
        if(BitsetToSigned32(temp)>0)rd_is_pos = pos;
        else if(BitsetToSigned32(temp)<0)rd_is_pos = neg;
        if (rt_is_pos==rs_is_pos &&rs_is_pos!=rd_is_pos) {//means overflow
            //print out sth and cont. the program.
            
        }
    }
}
//unsigned,(1)
void Reg::addu(long rs,long rt,long rd,int cycle){
    if (rd==0) {
        return;
    }
    this->reg[rd] = this->reg[rs]+this->reg[rt];
}
//signed,(1)(2)
void Reg::sub(long rs,long rt,long rd,int cycle){
    //A+(-B) using two's com to get -B first
    if (rd==0) {
    }
    int rs_is_pos=zero,minus_rt_is_pos=zero,rd_is_pos=zero;
    bitset<32>minus_rt = this->reg[rt].flip()+bitset<32>(1);//two's complemet-->flip back
    this->reg[rt].flip();
    if(BitsetToSigned32(minus_rt)>0)minus_rt_is_pos=pos;
    else if(BitsetToSigned32(minus_rt)<0)minus_rt_is_pos=neg;
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos=pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos=neg;
    if (rd!=0) {
        this->reg[rd] = this->reg[rs]+minus_rt;
        if(BitsetToSigned32(this->reg[rd])>0)rd_is_pos=pos;
        else if(BitsetToSigned32(this->reg[rd])<0)rd_is_pos=neg;
        if (minus_rt_is_pos==rs_is_pos &&rs_is_pos!=rd_is_pos) {//means overflow
        }
    }
    else{
        bitset<32>temp = this->reg[rs]+minus_rt;
        if(BitsetToSigned32(temp)>0)rd_is_pos=pos;
        else if(BitsetToSigned32(temp)<0)rd_is_pos=neg;
        if (minus_rt_is_pos==rs_is_pos &&rs_is_pos!=rd_is_pos) {//means overflow
        }
    }
}
void Reg::And(long rs,long rt,long rd,int cycle){
    if (rd==0) {
        return;
    }
    this->reg[rd] = this->reg[rs]&this->reg[rt];
}
void Reg::Or(long rs,long rt,long rd,int cycle){
    if (rd==0) {
        return;
    }
    this->reg[rd] = this->reg[rs]|this->reg[rt];
}
void Reg::Xor(long rs,long rt,long rd,int cycle){
    if (rd==0) {
        return;
    }
    this->reg[rd] = this->reg[rs]^this->reg[rt];
}
void Reg::Nor(long rs,long rt,long rd,int cycle){
    if (rd==0) {
        return;
    }
    this->reg[rd] = ~(this->reg[rs]|this->reg[rt]);
}
void Reg::Nand(long rs,long rt,long rd,int cycle){
    if (rd==0) {
        return;
    }
    this->reg[rd] = ~(this->reg[rs]&this->reg[rt]);
}

void Reg::slt(long rs,long rt,long rd,int cycle){
    if (rd==0) {
        return;
    }
    if(BitsetToSigned32(this->reg[rs])<BitsetToSigned32(this->reg[rt]))this->reg[rd] = 1;
    else this->reg[rd] = 0;
}
void Reg::sll(long rd,long rt,long C,int cycle){
    if (rd==0) {
        if(rt==0 && C==0)return;
        else {
            return;
        }
    }
    this->reg[rd] = this->reg[rt] << C;
}
void Reg::srl(long rd,long rt,long C,int cycle){
    if (rd==0) {
        return;
    }
    this->reg[rd] = this->reg[rt] >> C;
}
void Reg::sra(long rd,long rt,long C,int cycle){
    if (rd==0) {
        return;
    }
    bitset<32> a(this->reg[rt]);
    if (a[31]) {
        a = a>>C;
        for (int i = 0; i < C; i++) {
            a[31-i].flip();
        }
    }
    else a = a>>C;
    this->reg[rd] = a;
}

void Reg::jr(long rs,int cycle){
    this->reg[34] = this->reg[rs];//pc = reg[34]
    PCisadjusted = true;
}


//operations define below
//error def :
//(1)write$0 (2)Number overflow (3)Overwrite HI-LO registers
//(4)Memory address overflow (5)Data misaligned

//signed,(3)
void Reg::mult(long rs,long rt,int cycle){//may cause problem
    bitset<64>a,b;//rs,rt signed extension(a : rs/ b : rt)
    bitset<64>result(0),temp(0);
    if (ismult) {
        
    }
    for (int i = 0; i < 64; i++) {//signed extension.
        if(i<32){
            a[i] = this->reg[rs][i];
            b[i] = this->reg[rt][i];
        }
        else{
            a[i] = a[31];
            b[i] = b[31];
        }
    }
    for(int i = 0;i <64 ;i++){
        if(a[0]){
            temp = b;
            temp = temp << i;
            result=result+temp;
        }
    }
    for (int i = 0; i < 32;i++) {
        this->reg[32][i] = result[32+i];
        this->reg[33][i] = result[i];
    }
    ismult = true;
    //error detections.
}

//unsigned,(3)
void Reg::multu(long rs,long rt,int cycle){
    bitset<64>a(0),b(0);//rs,rt 0 extension(a : rs/ b : rt)
    bitset<64>result(0),temp(0);
    if (ismult) {
       
    }
    for (int i = 0; i < 32; i++) {
        a[i] = this->reg[rs][i];
        b[i] = this->reg[rt][i];
    }
    for(int i = 0;i <32 ;i++){
        if (a[i]) {
            temp = b;
            temp = temp << i;
            result=result+temp;
        }
    }
    for (int i = 0; i < 32;i++) {
        this->reg[32][i] = result[32+i];
        this->reg[33][i] = result[i];
    }
    ismult = true;
    //error detections.
}
//(1)
void Reg::mfhi(long rd,int cycle){
    //bf detect write 0-->change ismult to false
    ismult = false;
    if (rd==0) {
        
        return;
    }
    this->reg[rd] = this->reg[32];
}
void Reg::mflo(long rd,int cycle){
    ismult = false;
    if (rd==0) {
        
        return;
    }
    this->reg[rd] = this->reg[33];
}
// I inst.

//(1)(2)//signed
void Reg::addi(long rt, long rs, long i,int cycle){
    if (rt==0) {
        
    }
    int i_is_pos=zero,rs_is_pos=zero,rt_is_pos=zero;
    if(i > 0)i_is_pos = pos;
    else if(i < 0)i_is_pos = neg;
    if(this->BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(this->BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    
    bitset<32>im(i);//signed extension
    if (rt!=0) {
        this->reg[rt] = this->reg[rs]+im;
        if(this->BitsetToSigned32(this->reg[rt])>0)rt_is_pos=pos;
        else if(this->BitsetToSigned32(this->reg[rt])<0)rt_is_pos=neg;
        if (rs_is_pos==i_is_pos &&i_is_pos!=rt_is_pos) {
            //overflow
            
        }
    }
    else {
        bitset<32>temp = this->reg[rs]+im;
        if(this->BitsetToSigned32(temp)>0)rt_is_pos=pos;
        if(this->BitsetToSigned32(temp)<0)rt_is_pos=neg;
        if (rs_is_pos==i_is_pos &&i_is_pos!=rt_is_pos) {
            //overflow
            
        }
    }
}
//unsigned,(1)
void Reg::addiu(long rt, long rs, long i,int cycle){
    if (rt==0) {
        
        return;
    }
    bitset<32>im(i);//signed extension
    this->reg[rt] = this->reg[rs]+im;
}

//(1)(2)(4)(5)
//the memory index : 0,4,8,..
bitset<32> Reg::lw(long rt, long i, long rs, int cycle){
    //$t = 4 bytes from Memory[$s + C(signed)]
    //check C is a multiple of 4
    //check if $s+C is not available --> don't know reg[rs] is viewed as signed or not
    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    bitset<32>im(i);//signed extension.
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(this->reg[rs]+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    
    if (rt==0) {//(1)
        
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
            
        }
        if ((unsigned long long)(index.to_ulong()+3) > 1023 || BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
        else {
            if(index.to_ulong()%4!=0){
                halt = true;
            }
            // else this->reg[rt] = m->AdrToMem[int(index.to_ulong())]; // $0 cant write
        }
    }
    else {
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
        }
        if ((unsigned long long)(index.to_ulong()+3) > 1023 || BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
        else {
            if(index.to_ulong()%4!=0){
                halt = true;
            }
            else this->reg[rt] = D_disk[int(index.to_ulong())];
        }
    }
    return index;
}

//(1)(2)(4)(5)
bitset<32> Reg::lh(long rt, long i, long rs,int cycle){
    //$t = 2 bytes from Memory[$s + C(signed)] and rt should be signed extend.
    //check C is a multiple of 2
    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    bitset<32>im(i);//signed extension.
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(this->reg[rs]+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    
    if (rt==0) {//(1)
        
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
            
        }
        if ((unsigned long long)(index.to_ulong()+1) > 1023|| BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
        else {
            if(index.to_ulong()%2!=0){
                halt = true;
            }
        }
    }
    else {
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
            
        }
        if ((unsigned long long)(index.to_ulong()+1) > 1023|| BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
        else {
            if(index.to_ulong()%2!=0){
                halt = true;
            }
            else {
                int which_data = int(index.to_ulong()/4*4);
                int begin = int(index.to_ulong()%4);//0 or 2
                bitset<32>temp = D_disk[which_data];//load half
                for (int i = 0; i < 32; i++) {
                    if (i < 16) {
                        this->reg[rt][i] = temp[16-begin*8+i];
                    }
                    else this->reg[rt][i]= this->reg[rt][15];//signed extension
                }
            }
        }
    }
    return index;
}


//(1) (2) (4) (5)
bitset<32> Reg::lhu(long rt, long i, long rs, int cycle){//lhu  $2, 0($0)
    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    bitset<32>im(i);//signed extension.
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(this->reg[rs]+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    if (rt==0) {//(1)
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
        }
        if ((unsigned long long)(index.to_ulong()+1) > 1023|| BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
        else {
            if(index.to_ulong()%2!=0){
                halt = true;
            }
        }
    }
    else {
        if (rs_is_pos==i_is_pos && i_is_pos!=index_is_pos) {//(2)
        }
        if ((unsigned long long)(index.to_ulong()+1) > 1023|| BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
        else {
            if(index.to_ulong()%2!=0){
                halt = true;
            }
            else {
                int which_data = int(index.to_ulong()/4*4);
                int begin = int(index.to_ulong()%4);//0 or 2
                bitset<32>temp = D_disk[which_data];//load half
                for (int i = 0; i < 32; i++) {
                    if (i < 16) {
                        this->reg[rt][i] = temp[16-begin*8+i];
                    }
                    else this->reg[rt][i]= 0;//extension
                }
            }
        }
    }
    return index;
}
//(1) (2) (4)
bitset<32> Reg::lb(long rt, long i, long rs, int cycle){
    //$t = 1 bytes from Memory[$s + C(signed)] and rt should be signed extend.
    //check C is a multiple of 1-->don't check
    //check if $s+C is not available --> don't know reg[rs] is viewed as signed or not
    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    
    bitset<32>im(i);//signed extension.
    //     << this->reg[rt] << endl;
    
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(this->reg[rs]+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    if (rt==0) {//(1)
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
        }
        if ((unsigned long long)(index.to_ulong()) > 1023|| BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
    }
    else {
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
        }
        if ((unsigned long long)(index.to_ulong()) > 1023|| BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
        else {
            int which_data = int(index.to_ulong()/4*4);
            int begin = int(index.to_ulong()%4);//0 1 2
            bitset<32>temp = D_disk[which_data];//load byte
            for (int i = 0; i < 32; i++) {
                if (i < 8) {
                    this->reg[rt][i] = temp[24-begin*8+i];
                }
                else this->reg[rt][i]= this->reg[rt][7];//extension
            }
        }
    }
    return index;
}

bitset<32> Reg::lbu(long rt, long i, long rs,int cycle){
    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    bitset<32>im(i);//signed extension.
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(this->reg[rs]+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    
    if (rt==0) {//(1)
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
        }
        if ((unsigned long long)(index.to_ulong()) > 1023|| BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
    }
    else {
        if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
        }
        if ((unsigned long long)(index.to_ulong()) > 1023|| BitsetToSigned32(index) < 0) {//(4)
            halt = true;
        }
        else {
            int which_data = int(index.to_ulong()/4*4);
            int begin = int(index.to_ulong()%4);//0 or 2
            bitset<32>temp = D_disk[which_data];//load half
            for (int i = 0; i < 32; i++) {
                if (i < 8) {
                    this->reg[rt][i] = temp[24-begin*8+i];
                }
                else this->reg[rt][i]= 0;//extension
            }
        }
    }
    return index;
}

//(2) (4) (5)
bitset<32> Reg::sw(long rt, long i, long rs,int cycle){
    //4 bytes from Memory[$s + C(signed)] = $t
    //C is a multiple of 4?!
    //check adr is available or not.
    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    bitset<32>im(i);//signed extension.
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(this->reg[rs]+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    
    if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
    }
    if ((unsigned long long)index.to_ulong()+3>1023|| BitsetToSigned32(index) < 0) {//(4)
        halt = true;
        if (index.to_ulong()%4!=0) {
        }
    }
    else {
        if (index.to_ulong()%4!=0) {
            halt = true;
        }
        else D_disk[int(index.to_ulong())] = this->reg[rt];
    }
    return index;
}

//(2) (4) (5)
bitset<32> Reg::sh(long rt, long i, long rs, int cycle){
    //2 bytes from Memory[$s + C(signed)] = $t & 0x0000FFFF
    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    bitset<32>im(i);//signed extension.
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(this->reg[rs]+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    
    if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
    }
    if ((unsigned long long)index.to_ulong()+1>1023|| BitsetToSigned32(index) < 0) {//(4)
        halt = true;
        if (index.to_ulong()%2!=0) {
        }
    }
    else {
        if (index.to_ulong()%2!=0) {
            halt = true;
        }
        else {
            int which_data = int(index.to_ulong()/4*4);
            int begin = int(index.to_ulong()%4);//0 or 2
            for (int i = 0; i < 16; i++) {
                D_disk[which_data][16-begin*8+i] = this->reg[rt][i];
            }
        }
    }
    return index;
}
//(2) (4)
bitset<32> Reg::sb(long rt, long i, long rs,int cycle){
    //1 bytes from Memory[$s + C(signed)] = $t & 0x000000FF
    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    bitset<32>im(i);//signed extension.
    if(BitsetToSigned32(this->reg[rs])>0)rs_is_pos = pos;
    else if(BitsetToSigned32(this->reg[rs])<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(this->reg[rs]+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    
    if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
    }
    if ((unsigned long long)index.to_ulong()>1023|| BitsetToSigned32(index) < 0) {//(4)
        halt = true;
    }
    else {
        int which_data = int(index.to_ulong()/4*4);
        int begin = int(index.to_ulong()%4);//0 1 2 3
        for (int i = 0; i < 8; i++) {
            D_disk[which_data][24-begin*8+i] = this->reg[rt][i];
        }
    }
    return index;
}


//(1)
void Reg::lui(long rt, bitset<16> x,int cycle){
    //$t = C << 16
    // The lower bits of the register are set to 0.
    if (rt==0) {
        return;
    }
    for (int i = 0; i < 16; i++) {
        this->reg[rt][i] = 0;
    }
    for (int i = 16; i < 32; i++) {
        this->reg[rt][i] = x[i-16];
    }
}

//(1)
void Reg::Andi(long rt, long rs, long i,int cycle){//unsigned
    if (rt==0) {
        return;
    }
    bitset<32>x(i); // signed extends
    this->reg[rt] = this->reg[rs] & x;
}

void Reg::Ori(long rt, long rs, long i,int cycle){//unsigned
    if (rt==0) {
        return;
    }
    bitset<32>x(i);
    this->reg[rt] = this->reg[rs] | x;
}

void Reg::Nori(long rt, long rs, long i,int cycle){//unsigned
    if (rt==0) {
        return;
    }
    bitset<32>x(i);
    this->reg[rt] = ~(this->reg[rs]| x);
}

//(1)
void Reg::slti(long rt, long rs, long i,int cycle){
    //    $t = ($s < C(signed) ), signed comparison
    if (rt==0) {
        return;
    }
    if (BitsetToSigned32(this->reg[rs]) < i) {
        this->reg[rt] = 1;
    }
    else this->reg[rt] = 0;
}

void Reg::beq(long rs, long rt, long i,int cycle){
    
    if (this->reg[rs]==this->reg[rt]) {
        this->reg[34] = this->reg[34]+bitset<32>(4)+(bitset<32>(i)<<2);//may cause problems
        PCisadjusted = true;

    }
}
void Reg::bne(long rs, long rt, long i,int cycle){
    //    if ($s != $t) go to PC+4+4*C(signed)
    if (this->reg[rs]!=this->reg[rt]) {
        this->reg[34] = this->reg[34]+bitset<32>(4)+(bitset<32>(i)<<2);//may cause problems
        PCisadjusted = true;
    }
}
void Reg::bgtz(long rs, long i,int cycle){
    //if $s > 0 go to PC+4+4*C(signed)
    if (BitsetToSigned32(this->reg[rs])> 0) {
        this->reg[34] = this->reg[34]+bitset<32>(4)+(bitset<32>(i)<<2);//may cause problems
        PCisadjusted = true;
    }
}
////////J inst.

//
void Reg::j(bitset<26> adr,int cycle){
    //    PC = (PC+4)[31:28] | 4*C(unsigned)
    bitset<32> C4(0);
    bitset<32> temp(0);
    for(int i = 0 ; i < 26;i++)C4[i+2] = adr[i];
    for(int i = 0 ; i < 4 ;i++)temp[28+i]=(this->reg[34]+bitset<32>(4))[28+i];
    this->reg[34] = temp | C4;
    PCisadjusted = true;
}
void Reg::jal(bitset<26> adr,int cycle){//jump to adr
    //$31 = PC + 4
    //    PC = (PC+4)[31:28] | 4*C(unsigned)
    this->reg[31] = this->reg[34]+bitset<32>(4);
    bitset<32> C4(0);//4*C
    bitset<32> temp(0);
    for(int i = 0 ; i < 26;i++)C4[i+2] = adr[i];
    //equals to adr << 2-->PC*4
    for(int i = 0 ; i < 4 ;i++)temp[28+i]=(this->reg[34]+bitset<32>(4))[28+i];
    //extract PC+4[31:32]
    this->reg[34] = temp | C4;
    PCisadjusted = true;
}

