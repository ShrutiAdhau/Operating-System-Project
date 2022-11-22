#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <ctime>
using namespace std;

class PCB{
public:
    int TTL;
    int jobID;
    int TLL;
    int TTC;
    int LLC;
};

class VM{
public:
    char buffer[40];
    char Memory[300][4];
    char IR[4], R[4];
    bool toggle;
    int IC, SI=0;
    int PI, TI;
    fstream newfile;
    ofstream op;
    //array for frames allocated
    int frames_allocated[30] = {0};
    int PTR, instr_block;

    PCB pcb;


    VM(){
        newfile.open("cp2.txt", ios::in); 
        assign_all();
    }

    int real_address(int va){
        int PTE, ra_og;
        PTE = PTR*10 + va/10;
        if(Memory[PTE][0]=='@'){
            PI = 3;
            return -1;
        }
        // operand error
        ra_og = ((Memory[PTE][2] - '0')*10 + (Memory[PTE][3] - '0'))*10 + va%10;
        return ra_og;
        
    }


    void assign_all(){
        for (int i = 0; i < 40; i++)
        {
            buffer[i] = '@';
        }
        for (int i = 0; i < 300; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                Memory[i][j] = '@';
            }
        }
        for (int i = 0; i < 4; i++)
        {
            IR[i] = '@';
            R[i] = '@';
        }
        IC = 0;
        toggle = true;
        SI = 0;
        PI = 0;
        TI = 0;
        pcb.TTC = 0;
        pcb.LLC = 0;
        pcb.jobID = 0;
        pcb.TTL = 0;
        pcb.TLL = 0;
    }

    void clear_buffer(){
        for (int i = 0; i < 40; i++)
        {
            buffer[i] = '@';
        }
    }

    int allocate(){
        cout<<"Allocate called"<<endl;
        srand(time(NULL));
        int r = rand()%30;
        while(frames_allocated[r]==1){
            r = rand()%30;
        }
        frames_allocated[r]=1;
        cout<<"\nAllocated block: "<<r<<"\n";
        int add1 = PTR*10;
        while(Memory[add1][0] != '@'){
            add1++;
        }
        Memory[add1][0] = '0'; 
        Memory[add1][1] = '0';
        Memory[add1][2] = r/10+'0';
        Memory[add1][3] = r%10+'0';

        return r;
    }

    void load(){
        // cc : to read lines from i/p and copying it in buffer
        
        if(newfile.is_open()){
            string cc;
            while (getline(newfile, cc)){ // read data from file object and put it into string.
                if(cc[0]=='$' && cc[1]=='A' && cc[2]=='M' && cc[3]=='J'){
                    assign_all();
                    IC = 0;
                    cout<<"AMJ spotted\n";
                    cout<<"Instructions loading started\n";
                    //pcb.jobID = 0;
                    for(int i=0;i<4;i++){
                        pcb.jobID = pcb.jobID + (cc[i+4] - '0')*pow(10,3-i);
                    }
                    cout<<"JOBID: ";
                    cout<<pcb.jobID<<endl;

                    //pcb.TTL = 0;
                    for(int i=0;i<4;i++){
                        pcb.TTL = pcb.TTL + (cc[i+8] - '0')*pow(10,3-i);
                    }
                    cout<<"TTL: ";
                    cout<<pcb.TTL<<endl;

                    //pcb.TLL = 0;
                    for(int i=0;i<4;i++){
                        pcb.TLL = pcb.TLL + (cc[i+12] - '0')*pow(10,3-i);
                    }
                    cout<<"TLL: ";
                    cout<<pcb.TLL<<endl;
                  
                    srand(time(NULL));
                    PTR = rand()%30;
                    frames_allocated[PTR]=1;

                    cout<<"Page table frame allocated\n";
                    cout<<"PTR : "<<PTR<<endl;
                    //continue;
                }
                else if(cc[0]=='$' && cc[1]=='D' && cc[2]=='T' && cc[3]=='A'){
                    cout<<"Data card read\n";

                    //continue;

                    clear_buffer();
                    cout<<"\n Calling MOSstartexe function\n";
                    MOSstartexe();
                    //continue;
                }
                
                else if(cc[0]=='$' && cc[1]=='E' && cc[2]=='N' && cc[3]=='D'){
                    cout<<"END card\n";
                    //assign_all();
                    //continue;
                }

                else{
                    instr_block = allocate();
                    int instr_add = instr_block*10;
                    cout<<"\n Block allocated to instructions: "<<instr_block<<"\n";
                    

                    cout << "Program Card: " << cc << "\n";
                    int length = cc.size();
                    clear_buffer();
                    for (int i = 0; i < length; i++)
                    {
                        buffer[i] = cc[i];
                    }
                    cout<<"Buffer: ";
                    for (int i = 0; i < 40; i++)
                    {   
                        cout << buffer[i];
                    }
                    cout<<"\n";
                    cout<<"Program Card loading starts\n";
                    
                
                    int buff = 0;
                    //loading from buffer to memory
                    while(buff<40 && buffer[buff]!='@'){
                        for(int j=0;j<4;j++){
                            if(buffer[buff]=='H'){
                                Memory[instr_add][j] = 'H';
                                buff++;
                                break;
                            }
                            Memory[instr_add][j] = buffer[buff];
                            buff++;
                            
                        }
                        instr_add++;

                    }

                }
    
            
            }
            newfile.close(); // close the file object.
        }
    }
    void printer(){
    // prints memory
    cout<<"\n\n\n Printer Function\n\n";
    cout << "\nMemory: \n";
    for (int i = 0; i < 300; i++)
    {
        cout<<i<<" : ";
        for (int j = 0; j < 4; j++)
        {
            cout << Memory[i][j];
        }
        cout << "\n \n";
    }
    cout<<"Frames allocated: \n";
    for(int i=0;i<30;i++){
        cout<<i<<" : "<<frames_allocated[i]<<"\n";
    }
    // prints buffer
    cout << "\nBuffer: \n";
    for (int i = 0; i < 40; i++)
    {
        cout << buffer[i];
    }
    cout << "\n";

    // prints IR
    cout << "Current Instruction Register: \n";
    for (int i = 0; i < 4; i++)
    {
        cout << IR[i];
    }
    cout << "\n";
    // prints R
    cout << "General Purpose Register: \n";
    for (int i = 0; i < 4; i++)
    {
        cout << R[i];
    }
    cout << "\n";
    // prints IC
    cout << "Instruction Counter: \n";
    cout << IC << "\n";
    cout << "Toggle: \n";
    cout << toggle<<"\n";
    cout<<"\n\n\n Printer Function Ends\n\n";
    // prints toggle
    }

    void MOSstartexe(){
    //startexecution or mos function
    IC = 0;
    cout<<"\n Calling executeUser function\n";
    executeUser();
    }

    void executeUser(){
        int ra = real_address(IC);
        while(IC<99 && Memory[ra][0]!='@'){

            for(int i=0;i<4;i++){
                IR[i] = Memory[ra][i];
            }
            cout<<"\n"<<"IR: ";

            //printing IR
            for(int i=0;i<4;i++){
                cout<<IR[i];
            }
            cout<<"\n";
            cout<<"\nIC: "<<IC<<"\n";
            IC++;
            ra = real_address(IC);

            //If LR, Load the register
            if(IR[0] == 'L' && IR[1]=='R'){
                int lr_ra = real_address((IR[2] - '0')*10 + (IR[3] - '0'));
                cout << "Loading register\n";
                if (PI==2)
                {
                    masterMode();
                }
                lr_ra = real_address((IR[2] - '0')*10 + (IR[3] - '0'));
                for(int i=0;i<4;i++){
                    R[i] = Memory[lr_ra][i];
                }
                pcb.TTC++;
            }
            //If SR, store from register to Memory
            else if(IR[0] == 'S' && IR[1]=='R'){
                cout << "Register to Memory";
                int temp_ra = real_address((IR[2] - '0')*10 + (IR[3] - '0'));
                if(PI == 3 || PI==2){
                    masterMode();
                }
                temp_ra = real_address((IR[2] - '0')*10 + (IR[3] - '0'));
                for(int i=0;i<4;i++){
                    Memory[temp_ra][i] = R[i];
                }
                pcb.TTC = pcb.TTC+2;
            }

            
            else if(IR[0]=='C' && IR[1]=='R'){
                int temp = 0;
                int cr_ra = real_address((IR[2] - '0')*10 + (IR[3] - '0'));
                if (PI==2)
                {
                    masterMode();
                }
                cr_ra = real_address((IR[2] - '0')*10 + (IR[3] - '0'));
                for(int i=0;i<4;i++){
                    if(Memory[cr_ra][i] == R[i]){
                        temp++;
                    }
                }
                if(temp==4){
                    //cout<<"\nTemp = 4\n";
                    toggle = true;
                }
                else{
                    //cout<<"\nTemp != 4\n";
                    toggle = false;
                    }
                pcb.TTC++;
            }

            else if(IR[0]=='B' && IR[1]=='T'){
                int bt_ra = real_address(IC);

                if (PI==2)
                {
                    masterMode();
                }

                if(toggle==true){
                    IC = (IR[2] - '0')*10 + (IR[3] - '0');
                    ra =  real_address(IC);
                }
                pcb.TTC++;
            }

            else if(IR[0]=='G' && IR[1]=='D'){
                    SI = 1;
                    cout<<"\nCalling mastermode : GD\n";
                    masterMode();
                    pcb.TTC = pcb.TTC + 2;     
            }

            else if(IR[0]=='P' && IR[1]=='D'){
                SI = 2;
                cout<<"\nPD seen si = 2\n";
                cout<<"\nCalling mastermode : PD\n";
                masterMode(); 
                pcb.TTC++;
            }

            else if(IR[0]=='H'){
                SI = 3;
                cout<<"\nCalling mastermode : H\n";
                masterMode();
                pcb.TTC++;
                // return; 
            }
            else{
                PI = 1;
                cout<<"IR: ";
                for(int i=0;i<4;i++){
                    cout<<IR[i];
                }
                cout<<"\nCalling mastermode : opcode error\n";
                masterMode();
            }
            cout<<"\nTTC: "<<pcb.TTC<<"\n";
            if(pcb.TTC>=pcb.TTL && IR[0]!='H'){
                TI = 2;
                cout<<"\nCalling mastermode : Time Limit Exceeded\n";
                masterMode();
            }
    
    }
    
            
    }

    void masterMode(){
        cout<<"\nPI: "<<PI<<endl;
        cout<<"SI: "<<SI<<endl;
        cout<<"TI: "<<TI<<endl;
        if(PI!=0){
            switch (PI)
            {
            case 1:
                if(TI==0){
                    halt(4);
                    return;
                }
                else if(TI==2){
                    halt(3);
                    halt(4);
                    return;
                }
                break;
            case 2:
                if(TI==0){
                    halt(5);
                    return;
                }
                else if(TI==2){
                    halt(3);
                    halt(5);
                    return;
                }
                break;
            case 3:
                if(TI==0){
                    if((IR[0]=='G' && IR[1]=='D') || (IR[0]=='S' && IR[1]=='R')){
                        allocate();
                        PI = 0;
                    }
                    else{
                        halt(6);
                        return;
                    }
                }
                else if(TI==2){
                    halt(3);
                    return;
                }
                break;
            }
            //PI = 0;
        }
        else{
            switch (SI)
            {
            case 0:
                if(TI==2){
                    halt(3);
                    return;
                }
            case 1:
                if(TI==0){
                    read();
                    SI=0;
                    return;
                }
                else if(TI==2){
                    halt(3);
                    SI = 0;
                    return;
                }
                
                break;
            case 2:
                if(TI==0){
                    write();
                    SI=0;
                }
                else if(TI==2){
                    write();
                    halt(3);
                    SI = 0;
                }
            
                break;
            case 3:
                if(TI==0){
                    halt(0);
                    SI = 0;
                    return;
                }
                else if(TI==2){
                    halt(0);
                    halt(3);
                    SI = 0;
                }
               
                break;
            
            }
            
        }

    }

    void read(){
        cout<<"Read func called\n";
        IR[3] = '0';
        string cc2;
        getline(newfile,cc2);
        //if cc is end card
        if(cc2[0]=='$'){
            halt(1);
        }
        int len = cc2.size();
        for(int i = 0;i<len;i++){
            buffer[i] = cc2[i];
        }
        int buff = 0;
        int mem_ptr = real_address((IR[2]-'0')*10);
        if(PI==3){
            masterMode();
            cout<<"\nMasterMode called for PI = 3 in read func\n";
        }
        mem_ptr = real_address((IR[2]-'0')*10);
        while(buff<40 && buffer[buff]!='@'){
            for(int i=0;i<4;i++){
                Memory[mem_ptr][i] = buffer[buff];
                buff++;
            }
            mem_ptr++;
        }
            // prints buffer
        cout << "\nBuffer: \n";
        for (int i = 0; i < 40; i++)
        {
            cout << buffer[i];
        }
        cout << "\n";
        clear_buffer();
        //executeUser();

    }
    
    void write(){
        pcb.LLC++;
        if(pcb.LLC>pcb.TLL){
            halt(2);
            return;
        }
        cout<<"Write func called\n";
        op.open("output.txt", std::fstream::app);
        
        IR[3] = '0';
        for (int i = real_address((IR[2] - '0')*10); i < real_address((IR[2] - '0')*10) + 10; i++){
            for (int j = 0; j < 4; j++){
                if(Memory[i][j]!='@'){
                    op << Memory[i][j];
                }
            }
        }
        clear_buffer();
        op<<"\n";
        op.close();
    }

    void halt(int em){
        op.open("output.txt", std::fstream::app);
        cout<<"halt called\n\n";
        IC = 100;
        switch (em)
        {
        case 0:
            cout<<"No Error\n";
            op<<"No Error\n";
            op<<"\n\n";
            break;
        case 1:
            cout<<"Out of Data\n";
            op<<"Out of Data\n";
            op<<"\n\n";
            break;
        case 2:
            cout<<"Line Limit Exceeded\n";
            op<<"Line Limit Exceeded\n";
            op<<"\n\n";
            break;
        case 3:
            cout<<"Time Limit Exceeded\n";
            op<<"Time Limit Exceeded\n";
            op<<"\n\n";
            break;
        case 4:
            cout<<"Operation Code Error\n";
            op<<"Operation Code Error\n";
            op<<"\n\n";
            break;
        case 5:
            cout<<"Operand Error\n";
            op<<"Operand Error\n";
            op<<"\n\n";
            break;
        case 6:
            cout<<"Invalid Page Fault\n";
            op<<"Invalid Page Fault \n";
            op<<"\n\n";
            break;
        }
        op<<"SI: "<<SI<<" PI: "<<PI<<" TI: "<<TI<<"\n";
        op<<"PTR: "<<PTR<<" TTC: "<<pcb.TTC<<" LLC: "<<pcb.LLC<<"\n";
        op<<"TLL: "<<pcb.TLL<<" TTL: "<<pcb.TTL<<" JobID: "<<pcb.jobID<<"\n\n\n";

        op.close();

    }

};

int main()
{
    VM vm1;
    //vm1.printer();
    vm1.load();
    vm1.printer();
    return 0;
}
