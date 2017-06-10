//
//  main.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/6.
//  Copyright © 2017年 York You. All rights reserved.
//
#include "hierarchy.hpp"
#include "register.hpp"
#include "instruction.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>


string HextoBinary(char);
char inttohex(int i);

string ReadfileGetCode(string filename)
{
    ifstream file(filename.c_str());
    streampos size=0;
    char *buffer_temp;
    string buffer;
    if(file.is_open()){
        file.seekg (0, ios::end);
        size = file.tellg();
        buffer_temp = new char[size];
        file.seekg(0,ios::beg);
        file.read(buffer_temp, size);
        for (int i = 0; i < size; i++) {
            //cout << buffer_temp[i] <<" ";
            int code = buffer_temp[i];
            char x1, x2;
            if(code<0) code = 256+code;//two's complement
            x2 = inttohex(code%16); code /= 16;
            x1 = inttohex(code);
            buffer.append(HextoBinary(x1));
            buffer.append(HextoBinary(x2));
        }
        file.close();
    }
    return buffer;
}

//Imem,Dmem,PageI,PageD,totoalIcache,blockIcache,setIassociativity
//                      totoalDcache,blockDcache,setDassociativity
//disk for 1K
int main(int argc, const char * argv[]) {//open data
    string Dbuffer,Ibuffer,Assembly;
    Dbuffer = ReadfileGetCode("dimage.bin");
    Ibuffer = ReadfileGetCode("iimage.bin");
    ofstream report("report.rpt"),snapshot("snapshot.rpt"),Ass("Code.s");
    //initialize vars
    size_Imem=64,size_Ipage=8,size_Itotalcache=16,size_Iblock=4,set_Iassociativity=4;
    size_Dmem=32,size_Dpage=16,size_Dtotalcache=16,size_Dblock=4,set_Dassociativity=1;
    for (int i = 1; i < argc; i++) {
        if(i==1)     size_Imem = atoi(argv[i]);
        else if(i==2)size_Dmem = atoi(argv[i]);
        else if(i==3)size_Ipage = atoi(argv[i]);
        else if(i==4)size_Dpage = atoi(argv[i]);
        else if(i==5)size_Itotalcache = atoi(argv[i]);
        else if(i==6)size_Iblock = atoi(argv[i]);
        else if(i==7)set_Iassociativity = atoi(argv[i]);
        else if(i==8)size_Dtotalcache = atoi(argv[i]);
        else if(i==9)size_Dblock = atoi(argv[i]);
        else if(i==10)set_Dassociativity = atoi(argv[i]);
    }
    Initial_Disk(Dbuffer, Ibuffer);
    Reg r(getiSP(),getiPC());
    Instruction in;
    in.StartInstrctuion(&r);
    //report
    report << WriteOutput();
    snapshot << in.getOut();
    Ass << in.getAss();
    //trace
    cout << "\nASS : \n" << in.getAss() << "\n\nOut:\n" << WriteOutput();
    return 0;
}
string HextoBinary(char ch){
    switch (ch) {
        case '0':return "0000";
        case '1':return "0001";
        case '2':return "0010";
        case '3':return "0011";
        case '4':return "0100";
        case '5':return "0101";
        case '6':return "0110";
        case '7':return "0111";
        case '8':return "1000";
        case '9':return "1001";
        case 'a':return "1010";
        case 'b':return "1011";
        case 'c':return "1100";
        case 'd':return "1101";
        case 'e':return "1110";
        case 'f':return "1111";
        default: return "0000";
    }
}
char inttohex(int i){
    if(i>=0 && i<= 9) return i+'0';
    else return i-10+'a';
}