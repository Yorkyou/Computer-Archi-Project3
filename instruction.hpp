//
//  instruction.hpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.
//

#ifndef instruction_hpp
#define instruction_hpp
//
//  instruction.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.
//
using namespace::std;
#include <iostream>
#include <sstream>
#include "register.hpp"


class Instruction{//implementation
    friend class Reg;
public:
    Instruction(){it=0;Ass="";;output="";std::fill(Oldreg,Oldreg+34,0);};
    void StartInstrctuion(Reg *r);
    void RecordRegAndUpdate(Reg *r,int cycle);
    string BinaryToHex(bitset<32>);
    std::string getAss(){return Ass;}
    std::string getOut(){return output;}
    int cycle;
private:
    bitset<32> Oldreg[35];
    int it;
    std::string output;
    std::string Ass;
};

#endif /* instruction_hpp */
