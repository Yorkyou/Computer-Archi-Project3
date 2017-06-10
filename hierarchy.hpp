//
//  hierarchy.hpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/16.
//  Copyright © 2017年 York You. All rights reserved.
//

#ifndef memory_hpp
#define memory_hpp

//
//  memory.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.

using namespace std;
#include <iostream>
#include <string>
#include <map>
#include <bitset>
#include <list>
#include <math.h>
//implementing the the hierarchy
#define Limit 500001
class block{
public:
    int tag,va;
    bool MRU,valid;
    block(int t = -1,bool m = false,bool v = false,int vA=-1){
        tag = t;
        MRU = m;
        valid = v;
        va = vA;
    }
};
int hit_ICache,miss_ICache,hit_DCache,miss_DCache,hit_ITLB,miss_ITLB;
int hit_DTLB,miss_DTLB,hit_IpageTable,miss_IpageTable,hit_DpageTable,miss_DpageTable;
int size_Imem,size_Ipage,size_Itotalcache,size_Iblock,set_Iassociativity;
int size_Dmem,size_Dpage,size_Dtotalcache,size_Dblock,set_Dassociativity;
bitset<32> initialSP,initialPC;
string output;


//Disk
std::map<int,bitset<32>> D_disk;
std::map<int,bitset<32>> I_disk;
int D_size,I_size;

//Mem,LRU-->index by PPA
int num_DPage,num_IPage;//決定上面兩個array的大小
int last_used_cycle_Dpage[1024],last_used_cycle_Ipage[1024];//==0 invalid

//PageTable
std::map<int,int>DPageTable;//VPA->PPA
std::map<int,int>IPageTable;//VPA->PPA
int entry_DPageTable,entry_IPageTable;


//TLB ,LRU , fully-associative
std::map<int,int>DTLB;//VPA->PPA
std::map<int,int>ITLB;//VPA->PPA
int entry_Dtlb,entry_Itlb;
int last_used_cycle_Dtlb[1024],last_used_cycle_Itlb[1024];//==0 means invalid

//Cache , Bits-Pseudo LRU : 一個block裡面存n個data??!
//index by PA

int num_Dblock,num_Iblock;
int num_Dset,num_Iset;
block** DCache,**ICache;

//Operation
bitset<32> getiSP();
bitset<32> getiPC();
void Find_instruction(bitset<32>,int);//接收VA
void Find_data(bitset<32>,int);
string WriteOutput(){
    output = "";
    output = output+ "ICache :\n"+"# hits: "+to_string(hit_ICache)+"\n# misses: "+to_string(miss_ICache)+"\n\nDCache :\n"+"# hits: "+to_string(hit_DCache)+"\n# misses: "+to_string(miss_DCache)+"\n\nITLB :\n"+"# hits: "+to_string(hit_ITLB)+"\n# misses: "+to_string(miss_ITLB)+"\n\nDTLB :\n"+"# hits: "+to_string(hit_DTLB)+"\n# misses: "+to_string(miss_DTLB)+"\n\nIPageTable :\n"+"# hits: "+to_string(hit_IpageTable)+"\n# misses: "+to_string(miss_IpageTable)+"\n\nDPageTable :\n"+"# hits: "+to_string(hit_DpageTable)+"\n# misses: "+to_string(miss_DpageTable);
    return output;
}

void Initial_Disk(string Dbuffer,string Ibuffer)
{
    D_disk.clear();
    I_disk.clear();
    int it=0;//go through buffer
    for (int index = 0; index < 1023; index+=4) {
        D_disk[4*index] = 0;
        I_disk[4*index] = 0;
    }
    
    bitset<32> data(Dbuffer.substr(it,32));
    initialSP = data;//SP done!
    it+= 32;
    data=bitset<32>(Dbuffer.substr(it,32));
    D_size = int(data.to_ulong());//d_size done
    
    //up to 1023****
    for (int index=0; index < D_size ;index++) {
        it+=32;
        data=bitset<32>(Dbuffer.substr(it,32));
        D_disk[4*index] = data;//注意bigendian!!!
    }
    //Instruction set initialization
    it = 0;
    data=bitset<32>(Ibuffer.substr(it,32));
    initialPC = data;//unsigned
    it+=32;
    
    data=bitset<32>(Ibuffer.substr(it,32));
    I_size = int(data.to_ulong());//i_size
    //up to 1023-->資料從initial pc開始填寫
    int index=int(initialPC.to_ullong());
    for (int i = 0; i < I_size; i++) {
        it+=32;
        data=bitset<32>(Ibuffer.substr(it,32));
        I_disk[index] = data;//270,274,...
        index+=4;
    }
    hit_DCache = 0;
    miss_DCache = 0;
    hit_DpageTable = 0;
    miss_DpageTable = 0;
    hit_DTLB = 0;
    miss_DTLB = 0;
    hit_ICache = 0;
    miss_ICache = 0;
    hit_IpageTable = 0;
    miss_IpageTable = 0;
    hit_ITLB = 0;
    miss_ITLB = 0;
    //mem initialization : LRU
    num_DPage = size_Dmem/size_Dpage;
    num_IPage = size_Imem/size_Ipage;
    for (int i = 0; i < num_DPage; i++) {
        last_used_cycle_Dpage[i] = 0;
    }
    for (int i = 0; i < num_IPage; i++) {
        last_used_cycle_Ipage[i] = 0;
    }
    //PageTable
    //at page fault-->VA is disk adr
    DPageTable.clear();
    IPageTable.clear();
    entry_DPageTable = 1024 / size_Dpage;
    entry_IPageTable = 1024 / size_Ipage;
    
    //TLB ,LRU , fully-associative
    DTLB.clear();
    ITLB.clear();
    entry_Dtlb = entry_DPageTable/4;
    entry_Itlb = entry_IPageTable/4;
    for (int i = 0; i < 1024; i++) {
        last_used_cycle_Dtlb[i] = 0;
        last_used_cycle_Itlb[i] = 0;
    }
    //Cache : MRU replacement
    num_Dblock = size_Dtotalcache/size_Dblock;
    num_Iblock = size_Itotalcache/size_Iblock;
    num_Dset = num_Dblock/set_Dassociativity;
    num_Iset = num_Iblock/set_Iassociativity;
    block t;
    DCache = new block*[num_Dset];
    for (int i = 0; i < num_Dset; i++) {
        DCache[i] = new block[set_Dassociativity];
        for (int j = 0; j < set_Dassociativity; j++) {
            DCache[i][j] = *new block();
        }
    }
    ICache = new block*[num_Iset];
    for (int i = 0; i < num_Iset; i++) {
        ICache[i] = new block[set_Iassociativity];
        for (int j = 0; j < set_Iassociativity; j++) {
            ICache[i][j] = *new block();
        }
    }
}
void Find_instruction(bitset<32> a,int cycle){
    //1:check TLB
    int v_adr = int(a.to_ulong());//virtual
    int p_adr,VPA,PPA,offset,size_offset;
    
    VPA = v_adr/size_Ipage;
    offset = v_adr%size_Ipage;
    size_offset = log2(size_Ipage);
    
    if (ITLB.count(VPA)) { // ITLB hit
        PPA = ITLB.at(VPA);
        last_used_cycle_Itlb[VPA] = cycle;
        hit_ITLB++;
    }
    else { //ITLB miss-->go to PageTable-->check
        miss_ITLB++;
        //find VPA(entry of PTE) and find PPA
        if (IPageTable.count(VPA)) {//Data is in PageTable but not in ITLB-->update ITLB
            PPA = IPageTable.at(VPA);
            hit_IpageTable++;
            if (ITLB.size() < entry_Itlb){//還沒滿直接放進去就好
                ITLB[VPA] = PPA;
                last_used_cycle_Itlb[VPA] = cycle;
            }
            else {//ITLB滿了要先replace舊的-->search last used cycle but != 0
                int which_to_replace = 0,num=Limit;
                for (auto x : ITLB) {
                    if (last_used_cycle_Itlb[x.first] < num &&last_used_cycle_Itlb[x.first]!=0) {
                        num = last_used_cycle_Itlb[x.first];
                        which_to_replace = x.first;
                    }
                }
                ITLB.erase(which_to_replace);
                last_used_cycle_Itlb[which_to_replace] = 0;
                ITLB[VPA] = PPA;
                last_used_cycle_Itlb[VPA] = cycle;
            }
        }
        else{//Page Fault-->(1)更新mem(2)update PageTable and ITLB(3)clean related PPA info
            miss_IpageTable++;
            //先去Memory找一塊空的範圍or replace
            bool replace = false;
            int which_to_use=0,num=Limit;
            for (int i = 0; i < num_IPage; i++) {
                if (last_used_cycle_Ipage[i]==0) {
                    which_to_use = i;
                    replace = false;
                    break;
                }
                else if (last_used_cycle_Ipage[i] <num && last_used_cycle_Ipage[i]!=0) {
                    which_to_use = i;//use == replace
                    num = last_used_cycle_Ipage[i];
                    replace = true;
                }
            }
            last_used_cycle_Ipage[which_to_use] = cycle;//代表被使用
            //update Page Table
            for (auto x : IPageTable) {
                if (x.second==which_to_use) {
                    IPageTable.erase(x.first);
                    break;
                }
            }
            IPageTable[VPA] = which_to_use;
            //update ITLB
            for (auto x : ITLB) {
                if (x.second==which_to_use) {
                    ITLB.erase(x.first);
                    break;
                }
            }
            if (ITLB.size() < entry_Itlb){//還沒滿直接放進去就好
                ITLB[VPA] = which_to_use;
                last_used_cycle_Itlb[VPA] = cycle;
            }
            else {//ITLB滿了要先replace舊的-->search last used cycle but != 0
                int which_to_replace = 0,num=Limit;
                for (auto x : ITLB) {
                    if (last_used_cycle_Itlb[x.first] <num&&last_used_cycle_Itlb[x.first]!=0) {
                        num = last_used_cycle_Itlb[x.first];
                        which_to_replace = x.first;
                    }
                }
                ITLB.erase(which_to_replace);
                last_used_cycle_Itlb[which_to_replace] = 0;
                ITLB[VPA] = which_to_use;
                last_used_cycle_Itlb[VPA] = cycle;
            }
            //update Cache-
            if (replace) {
                int size_byteoffset = log2(size_Iblock);//5
                int size_setoffset = log2(num_Iset);//6
                for (int i = 0; i < num_Iset; i++) {
                    for (int j = 0; j < set_Iassociativity; j++) {
                        int p_adr = ICache[i][j].tag*int(pow(2, size_setoffset+size_byteoffset));
                        int ppa = p_adr/pow(2, size_offset);
                        if(ppa==which_to_use){
                            ICache[i][j].MRU = false;
                            ICache[i][j].valid = false;
                        }
                    }
                }
            }
            PPA = which_to_use;
        }
    }
    //be able to get PPA here.
    p_adr = PPA*pow(2, size_offset)+offset;
    //get info
    int size_byteoffset = log2(size_Iblock);//5
    int size_setoffset = log2(num_Iset);//6
    int setindex = p_adr / int(pow(2, size_byteoffset)) % int(pow(2, size_setoffset));//error?
    int tag = p_adr / int(pow(2, size_setoffset+size_byteoffset));
    //check if data exist in ICache
    bool found = false;//all true用在滿的時候replacement policy
    for (int x = 0;x < set_Iassociativity;x++) {//needs to handle fully asso
        if (ICache[setindex][x].valid && ICache[setindex][x].tag==tag) {
            found = true;
            ICache[setindex][x].MRU = true;
        }
    }
    if(found) hit_ICache++;
    else {//not found in Cache -->把東西搬進來:maybe replace
        miss_ICache++;
        bool allTrue=true;
        for (int x=0; x < set_Iassociativity; x++) {
            if (set_Iassociativity==1) {
                block t(tag,true,true,v_adr);
                ICache[setindex][x] = t;
                break;
            }
            else if (ICache[setindex][x].MRU==false) {
                block t(tag,true,true,v_adr);
                ICache[setindex][x] = t;
                for (int y = 0; y < set_Iassociativity; y++) {
                    if (y!=x && ICache[setindex][y].MRU==false){
                        allTrue = false;
                        break;
                    }
                }
                if (allTrue) {
                    for (int y = 0; y < set_Iassociativity; y++) {
                        if (y!=x)ICache[setindex][y].MRU=false;
                    }
                }
                break;
            }
        }
    }
//        cout << "cycle ,PC " << cycle << " " << a.to_ulong() << endl;
//        cout << "ITLB hit , miss" << hit_ITLB << " " << miss_ITLB << endl;
//        cout << "Ipagetable hit , miss" << hit_IpageTable << " " << miss_IpageTable << endl;
//        cout << "Icache hit , miss " << hit_ICache << "  " << miss_ICache << endl;
//        cout << "\nITLB :\n";
//        for(auto i : ITLB){
//            cout << "key: " << i.first <<" value :"<<i.second << endl;
//        }
//        cout << "IPageTable :\n";
//        for(auto i : IPageTable) {
//            cout << "key: " << i.first <<" value :"<<i.second << endl;
//        }
//        cout << "ICache :\n";
//        for (int i = 0; i < num_Iset; i++) {
//            for (int j = 0; j < set_Iassociativity; j++) {
//                cout << "set " << i << "block " << j << " tag " << ICache[i][j].tag << " MRU " << ICache[i][j].MRU << " valid " << ICache[i][j].valid << " VA "<< ICache[i][j].va << endl;
//            }
//        }
//        cout << "\n";
}
void Find_data(bitset<32> a,int cycle){
    //1:check TLB
    int v_adr = int(a.to_ulong());//virtual
    int p_adr,VPA,PPA,offset,size_offset;
    
    VPA = v_adr/size_Dpage;
    offset = v_adr%size_Dpage;
    size_offset = log2(size_Dpage);
    
    if (DTLB.count(VPA)) { // DTLB hit
        PPA = DTLB.at(VPA);
        last_used_cycle_Dtlb[VPA] = cycle;
        hit_DTLB++;
    }
    else { //DTLB miss-->go to PageTable-->check
        miss_DTLB++;
        //find VPA(entry of PTE) and find PPA
        if (DPageTable.count(VPA)) {//Data is in PageTable but not in ITLB-->update ITLB
            PPA = DPageTable.at(VPA);
            hit_DpageTable++;
            if (DTLB.size() < entry_Dtlb){//還沒滿直接放進去就好
                DTLB[VPA] = PPA;
                last_used_cycle_Dtlb[VPA] = cycle;
            }
            else {//ITLB滿了要先replace舊的-->search last used cycle but != 0
                int which_to_replace = 0,num=Limit;
                for (auto x : DTLB) {
                    if (last_used_cycle_Dtlb[x.first] < num&&last_used_cycle_Dtlb[x.first]!=0) {
                        num = last_used_cycle_Dtlb[x.first];
                        which_to_replace = x.first;
                    }
                }
                DTLB.erase(which_to_replace);
                last_used_cycle_Dtlb[which_to_replace] = 0;
                DTLB[VPA] = PPA;
                last_used_cycle_Dtlb[VPA] = cycle;
            }
        }
        else{//Page Fault-->(1)更新mem(2)update PageTable and ITLB(3)clean related PPA info
            miss_DpageTable++;
            if (cycle==76) {
                cout << "hello";
            }
            //先去Memory找一塊空的範圍or replace
            int which_to_use=0,num=Limit;
            bool replace = false;
            for (int i = 0; i < num_DPage; i++) {
                if (last_used_cycle_Dpage[i]==0) {
                    which_to_use = i;
                    replace = false;
                    break;
                }
                else if (last_used_cycle_Dpage[i] < num&& last_used_cycle_Dpage[i]!=0) {
                    which_to_use = i;//use == replace
                    num = last_used_cycle_Dpage[i];
                    replace = true;
                }
            }
            last_used_cycle_Dpage[which_to_use] = cycle;//代表被使用
            //update Page Table
            
            for (auto x : DPageTable) {
                if (x.second==which_to_use) {
                    DPageTable.erase(x.first);
                    break;
                }
            }
            DPageTable[VPA] = which_to_use;
            //update DTLB
            for (auto x : DTLB) {
                if (x.second==which_to_use) {
                    DTLB.erase(x.first);
                    break;
                }
            }
            if (DTLB.size() < entry_Dtlb){//還沒滿直接放進去就好
                DTLB[VPA] = which_to_use;
                last_used_cycle_Dtlb[VPA] = cycle;
            }
            else {//ITLB滿了要先replace舊的-->search last used cycle but != 0
                int which_to_replace = 0,num=Limit;
                for (auto x : DTLB) {
                    if (last_used_cycle_Dtlb[x.first]<num&&last_used_cycle_Dtlb[x.first]!=0) {
                        num = last_used_cycle_Dtlb[x.first];
                        which_to_replace = x.first;
                    }
                }
                DTLB.erase(which_to_replace);
                last_used_cycle_Dtlb[which_to_replace] = 0;
                DTLB[VPA] = which_to_use;
                last_used_cycle_Dtlb[VPA] = cycle;
            }
            //update Cache
            if (replace) {
                int size_byteoffset = log2(size_Dblock);//5
                int size_setoffset = log2(num_Dset);//6
                for (int i = 0; i < num_Dset; i++) {
                    for (int j = 0; j < set_Dassociativity; j++) {
                        int p_adr = DCache[i][j].tag*int(pow(2, size_setoffset+size_byteoffset));
                        int ppa = p_adr/pow(2, size_offset);
                        if(ppa==which_to_use){
                            DCache[i][j].MRU = false;
                            DCache[i][j].valid = false;
                        }
                    }
                }
            }
            PPA = which_to_use;
        }
    }
    //be able to get PPA here.
    p_adr = PPA*pow(2, size_offset)+offset;
    //get info
    int size_byteoffset = log2(size_Dblock);//5
    int size_setoffset = log2(num_Dset);//6
    int setindex = p_adr / int(pow(2, size_byteoffset)) % int(pow(2, size_setoffset));//error?
    int tag = p_adr / int(pow(2, size_setoffset+size_byteoffset));
    //check if data exist in DCache
    bool found = false;//all true用在滿的時候replacement policy
    //Dcache output
    
    for (int x = 0;x < set_Dassociativity;x++) {//needs to handle fully asso
        if (DCache[setindex][x].valid && DCache[setindex][x].tag==tag) {
            found = true;
            DCache[setindex][x].MRU = true;
        }
    }
    if(found) hit_DCache++;
    else {//not found in Cache -->把東西搬進來:maybe replace
        miss_DCache++;
        bool allTrue=true;
        for (int x=0; x < set_Dassociativity; x++) {//direct map
            if (set_Dassociativity==1) {
                block t(tag,true,true,v_adr);
                DCache[setindex][x] = t;
                break;
            }
            else if (DCache[setindex][x].MRU==false) {
                block t(tag,true,true,v_adr);
                DCache[setindex][x] = t;
                for (int y = 0; y < set_Dassociativity; y++) {
                    if (y!=x && DCache[setindex][y].MRU==false){
                        allTrue = false;
                        break;
                    }
                }
                if (allTrue) {
                    for (int y = 0; y < set_Dassociativity; y++) {
                        if (y!=x)DCache[setindex][y].MRU=false;
                    }
                }
                break;
            }
        }
    }
        cout << "cycle ,PC " << cycle << " " << a.to_ulong() << endl;
        cout << "DTLB hit , miss" << hit_DTLB << " " << miss_DTLB << endl;
        cout << "Dpagetable hit , miss" << hit_DpageTable << " " << miss_DpageTable << endl;
        cout << "Dcache hit , miss " << hit_DCache << "  " << miss_DCache << endl;
        cout << "\nDTLB :\n";
        for(auto i : DTLB){
            cout << "key: " << i.first <<" value :"<<i.second << endl;
        }
        cout << "DPageTable :\n";
        for(auto i : DPageTable) {
            cout << "key: " << i.first <<" value :"<<i.second << endl;
        }
        cout << "DCache :\n";
        for (int i = 0; i < num_Dset; i++) {
            for (int j = 0; j < set_Dassociativity; j++) {
                cout << "set " << i << "block " << j << " tag " << DCache[i][j].tag << " MRU " << DCache[i][j].MRU << " valid " << DCache[i][j].valid << " VA "<< DCache[i][j].va << endl;
            }
        }
        cout << "\n";
    
}
bitset<32> getiSP(){return initialSP;}
bitset<32> getiPC(){return initialPC;}
//long Hierarchy::getIsize(){return I_size;}


#endif /* memory_h */
