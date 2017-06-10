//
//  instruction.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.
//
using namespace::std;
#include <iostream>
#include <map>
#include <algorithm>
#include "instruction.hpp"
extern std::map<int,bitset<32>> D_disk,I_disk;
extern void Find_instruction(bitset<32> a,int cycle);
extern void Find_data(bitset<32> a,int cycle);
string Instruction::BinaryToHex(bitset<32>a){
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
void Instruction::RecordRegAndUpdate(Reg* r, int cycle){
    if (cycle==0) {
        output += "cycle 0\n";
        for (int i = 0; i < 35; i++) {
            if (i==32)
                output += "$HI: "+BinaryToHex(r->reg[i])+"\n";
            else if (i==33)
                output += "$LO: "+BinaryToHex(r->reg[i])+"\n";
            else if (i==34)
                output += "PC: "+BinaryToHex(r->reg[i])+"\n";
            else
                output += "$"+(i<10 ?to_string(0)+to_string(i) :to_string(i))+": "+BinaryToHex(r->reg[i])+"\n";
            //initialize old reg
            this->Oldreg[i] = r->reg[i];
        }
        output +="\n\n";
    }
    else{
        output += "cycle "+to_string(cycle)+"\n";
        for (int i = 0; i < 35; i++) {
            if (r->reg[i]!=this->Oldreg[i]) {
                if (i==32)
                    output += "$HI: "+BinaryToHex(r->reg[i])+"\n";
                else if (i==33)
                    output += "$LO: "+BinaryToHex(r->reg[i])+"\n";
                else if (i==34)
                    output += "PC: "+BinaryToHex(r->reg[i])+"\n";
                else
                    output += "$"+(i<10 ?to_string(0)+to_string(i) :to_string(i))+": "+BinaryToHex(r->reg[i])+"\n";
            }
            this->Oldreg[i] = r->reg[i];
        }
        output +="\n\n";
    }
}
void Instruction::StartInstrctuion(Reg *r){
    bitset<32> command,index=0;
    cycle = 0;
    // r->reg[34] As PC -->maybe there is signed / unsigend -->print out data bf execution
    // If PC is adjusted in command -->don't need to add four
    for (command=bitset<32>(I_disk[int(r->reg[34].to_ulong())])
         ;!command.none()|| r->reg[34].to_ulong() < 1024
         ;command=bitset<32>(I_disk[int(r->reg[34].to_ulong())]),cycle++) {
        //halt by command or other accident --> updata reg[34] after each inst.
        RecordRegAndUpdate(r, cycle);
        //Check where to get "command"
        Find_instruction(r->reg[34],cycle+1);
        
        int opcode = int((command>>26).to_ulong());
        int funct = int((command<<26>>26).to_ulong());
        //get rs,rt,rd.(reg),shamt-->they must be positive so we directly trnaslate to int
        //immediate must be + or -
        int rs = int((command<<6>>27).to_ulong());
        int rt = int((command<<11>>27).to_ulong());
        int rd = int((command<<16>>27).to_ulong());
        int shamt = int((command<<21>>27).to_ulong());

        bitset<32> temp1 = command<<16>>16;//used for immediate
        bitset<32> temp2 = command<<6>>6;//used for adr
        bitset<16> immediate;
        bitset<26> adress;
        for (int i = 0; i < 16; i++) immediate[i] = temp1[i];
        for (int i = 0; i < 26; i++) adress[i] = temp2[i];
        if (command.to_ulong()==0) {//its a NOP
            r->PCmoveForward();
            continue;
        }

        switch (opcode) {
            case 0x00: //R-type Instructions
                switch (funct) {
                    case 0x20://add
                        Ass+=to_string(cycle)+"add $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->add(rs, rt, rd,cycle+1);
                        break;
                    case 0x21://addu
                        Ass+=to_string(cycle)+"addu $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->addu(rs, rt, rd,cycle+1);
                        break;
                    case 0x22://sub
                        Ass+=to_string(cycle)+"sub $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->sub(rs, rt, rd,cycle+1);
                        break;
                    case 0x24://and
                        Ass+=to_string(cycle)+"and $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->And(rs, rt, rd,cycle+1);
                        break;
                    case 0x25://or
                        Ass+=to_string(cycle)+"or $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->Or(rs, rt, rd,cycle+1);
                        break;
                    case 0x26://xor
                        Ass+=to_string(cycle)+"xor $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->Xor(rs, rt, rd,cycle+1);
                        break;
                    case 0x27://nor
                        Ass+=to_string(cycle)+"nor $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->Nor(rs, rt, rd,cycle+1);
                        break;
                    case 0x28://nand
                        Ass+=to_string(cycle)+"nand $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->Nand(rs, rt, rd,cycle+1);
                        break;
                    case 0x2A://slt
                        Ass+=to_string(cycle)+"slt $"+to_string(rd)+",$"+to_string(rs)+",$"+to_string(rt)+"\n";
                        r->slt(rs, rt, rd,cycle+1);
                        break;
                    case 0x00://sll
                        Ass+=to_string(cycle)+"sll $"+to_string(rd)+",$"+to_string(rt)+" "+to_string(shamt)+"\n";
                        r->sll(rd, rt, shamt,cycle+1);
                        break;
                    case 0x02://srl
                        Ass+=to_string(cycle)+"srl $"+to_string(rd)+",$"+to_string(rt)+" "+to_string(shamt)+"\n";
                        r->srl(rd, rt, shamt,cycle+1);
                        break;
                    case 0x03://sra
                        Ass+=to_string(cycle)+"sra $"+to_string(rd)+",$"+to_string(rt)+" "+to_string(shamt)+"\n";
                        r->sra(rd, rt, shamt,cycle+1);
                        break;
                    case 0x08://jr
                        Ass+=to_string(cycle)+"jr $"+to_string(rs)+"\n";
                        r->jr(rs,cycle+1);
                        break;
                    case 0x18:
                        Ass+=to_string(cycle)+"mult $"+to_string(rs)+" $"+to_string(rt)+"\n";
                        r->mult(rs,rt,cycle+1);
                        break;
                    case 0x19:
                        Ass+=to_string(cycle)+"multu $"+to_string(rs)+" $"+to_string(rt)+"\n";
                        r->multu(rs,rt,cycle+1);
                        break;
                    case 0x10:
                        Ass+=to_string(cycle)+"mfhi $"+to_string(rd)+"\n";
                        r->mfhi(rd,cycle+1);
                        break;
                    case 0x12:
                        Ass+=to_string(cycle)+"mflo $"+to_string(rd)+"\n";
                        r->mflo(rd,cycle+1);
                        break;
                    default:
                        cout << "illegal instruction found at "<< BinaryToHex(r->reg[34]) << endl;
                        r->halt = true;
                        // no such Instructions.
                        // halt.
                        break;
                }
                break;
                //I instructions start from here.
            case 0x08:
                Ass+=to_string(cycle)+"addi $"+to_string(rt)+",$"+to_string(rs)+","+to_string(r->BitsetToSigned16(immediate))+"\n";
                r->addi(rt,rs,r->BitsetToSigned16(immediate),cycle+1);
                break;
            case 0x09:
                Ass+=to_string(cycle)+"addiu $"+to_string(rt)+",$"+to_string(rs)+","+to_string(immediate.to_ulong())+"\n";
                r->addiu(rt,rs,r->BitsetToSigned16(immediate),cycle+1);
                break;
            case 0x23://lw
                Ass+=to_string(cycle)+"lw $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"($"+to_string(rs)+")"+"\n";
                index = r->lw(rt, r->BitsetToSigned16(immediate), rs,cycle+1);
                Find_data(index,cycle+1);
                break;
            case 0x21://lh
                Ass+=to_string(cycle)+"lh $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"($"+to_string(rs)+")"+"\n";
                index = r->lh(rt, r->BitsetToSigned16(immediate), rs,cycle+1);
                Find_data(index,cycle+1);
                break;
            case 0x25:
                Ass+=to_string(cycle)+"lhu $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"($"+to_string(rs)+")"+"\n";
                index = r->lhu(rt, r->BitsetToSigned16(immediate), rs,cycle+1);
                Find_data(index,cycle+1);
                break;
            case 0x20://lb
                Ass+=to_string(cycle)+"lb $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"($"+to_string(rs)+")"+"\n";
                index = r->lb(rt, r->BitsetToSigned16(immediate), rs,cycle+1);
                Find_data(index,cycle+1);
                break;
            case 0x24://lbu
                Ass+=to_string(cycle)+"lbu $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"($"+to_string(rs)+")"+"\n";
                index = r->lbu(rt, r->BitsetToSigned16(immediate), rs,cycle+1);
                Find_data(index,cycle+1);
                break;
            case 0x2B://sw
                Ass+=to_string(cycle)+"sw $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"($"+to_string(rs)+")"+"\n";
                index = r->sw(rt, r->BitsetToSigned16(immediate), rs,cycle+1);
                Find_data(index,cycle+1);
                break;
            case 0x29://sh
                Ass+=to_string(cycle)+"sh $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"($"+to_string(rs)+")"+"\n";
                index = r->sh(rt, r->BitsetToSigned16(immediate), rs,cycle+1);
                Find_data(index,cycle+1);
                break;
            case 0x28://sb
                Ass+=to_string(cycle)+"sb $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"($"+to_string(rs)+")"+"\n";
                index = r->sb(rt, r->BitsetToSigned16(immediate), rs,cycle+1);
                Find_data(index,cycle+1);
                break;
            case 0x0F://lui
                Ass+=to_string(cycle)+"lui $"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"\n";
                r->lui(rt, immediate,cycle+1);
                break;
            case 0x0C://andi
                Ass+=to_string(cycle)+"andi $"+to_string(rt)+",$"+to_string(rs)+","+to_string(immediate.to_ulong())+"\n";
                r->Andi(rt, rs, immediate.to_ulong(),cycle+1);
                break;
            case 0x0D://ori
                Ass+=to_string(cycle)+"ori $"+to_string(rt)+",$"+to_string(rs)+","+to_string(immediate.to_ulong())+"\n";;
                r->Ori(rt, rs, immediate.to_ulong(),cycle+1);
                break;
            case 0x0E://nori
                Ass+=to_string(cycle)+"nori $"+to_string(rt)+",$"+to_string(rs)+","+to_string(immediate.to_ulong())+"\n";
                r->Nori(rt, rs, immediate.to_ulong(),cycle+1);
                break;
            case 0x0A://slti
                Ass+=to_string(cycle)+"slti $"+to_string(rt)+",$"+to_string(rs)+","+to_string(r->BitsetToSigned16(immediate))+"\n";
                r->slti(rt, rs, r->BitsetToSigned16(immediate),cycle+1);
                break;
            case 0x04://beq
                Ass+=to_string(cycle)+"beq $"+to_string(rs)+",$"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"\n";
                r->beq(rs, rt, r->BitsetToSigned16(immediate),cycle+1);
                break;
            case 0x05://bne
                Ass+=to_string(cycle)+"bne $"+to_string(rs)+",$"+to_string(rt)+","+to_string(r->BitsetToSigned16(immediate))+"\n";
                r->bne(rs, rt, r->BitsetToSigned16(immediate),cycle+1);
                break;
            case 0x07://bgtz
                Ass+=to_string(cycle)+"bgtz $"+to_string(rs)+","+to_string(r->BitsetToSigned16(immediate))+"\n";
                r->bgtz(rs, r->BitsetToSigned16(immediate),cycle+1);
                break;
            //////////J-type Inst.
            case 0x02://j
                Ass+=to_string(cycle)+"j "+to_string(adress.to_ulong())+"\n";
                r->j(adress,cycle+1);
                break;
            case 0x03://jal
                Ass+=to_string(cycle)+"jal "+to_string(adress.to_ulong())+"\n";
                r->jal(adress,cycle+1);
                break;
                
            /////////////halt
            case 0x3F:
                Ass+=to_string(cycle)+"halt";
                r->halt = true;
                break;
            default:
                cout << "illegal instruction found at "<< BinaryToHex(r->reg[34]) << endl;
                r->halt = true;
                //no such instructions.
                break;
        }
        if (r->halt)break;
        if (!r->PCisadjusted) {
            r->PCmoveForward();
        }
        else r->PCisadjusted = false;
        //halt ~~~
    }
}
