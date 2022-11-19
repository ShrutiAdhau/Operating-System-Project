#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class VM{
public:
    char buffer[40];
    char Memory[100][4];
    char IR[4], R[4];
    bool toggle;
    int IC, SI=0;
    int memory_ptr = 0;
    fstream newfile;
    ofstream op;


    VM(){
        newfile.open("input_1.txt", ios::in); 
        assign_all();
    }

    void assign_all(){
        for (int i = 0; i < 40; i++)
        {
            buffer[i] = '@';
        }
        for (int i = 0; i < 100; i++)
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
        memory_ptr = 0;
        SI = 0;
    }

    void clear_buffer(){
        for (int i = 0; i < 40; i++)
        {
            buffer[i] = '@';
        }
    }
    void load(){
        // cc : to read lines from i/p and copying it in buffer
        
        if(newfile.is_open()){ 
            string cc;
            while (getline(newfile, cc)){ // read data from file object and put it into string.
                if(cc[0]=='$' && cc[1]=='A' && cc[2]=='M' && cc[3]=='J'){
                    assign_all();
                    cout<<"AMJ spotted\n";
                    cout<<"Instructions loading started\n";
                    
                }
                else if(cc[0]=='$' && cc[1]=='D' && cc[2]=='T' && cc[3]=='A'){
                    cout<<"Data card read\n";

                    clear_buffer();
                    MOSstartexe();
                   
                }
                
                else if(cc[0]=='$' && cc[1]=='E' && cc[2]=='N' && cc[3]=='D'){
                    cout<<"END card\n";
                    //printer();
                    //assign_all();
                
                }

                else{
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
                    int ref = 0;
                    //loading from buffer to memory
                    while(buff<40 && buffer[buff]!='@'){
                        for(int j=0;j<4;j++){
                            if(buffer[buff]=='H'){
                                Memory[memory_ptr][j] = 'H';
                                buff++;
                                break;
                            }
                            Memory[memory_ptr][j] = buffer[buff];
                            buff++;
                            
                        }
                        memory_ptr++;

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
    for (int i = 0; i < 100; i++)
    {
        if(i>=0 && i<=9){
            cout <<"0"<< i << ": ";
        }
        else{
            cout << i << ": ";
        }
        for (int j = 0; j < 4; j++)
        {
            cout << Memory[i][j];
        }
        cout << "\n \n";
    }

    // prints buffer
    cout << "Buffer: \n";
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
    executeUser();
    }
    void executeUser(){
        while(IC<99 && Memory[IC][0] !='@'){

            for(int i=0;i<4;i++){
                IR[i] = Memory[IC][i];
            }
            cout<<"\n"<<"IR: ";

            //printing IR
            for(int i=0;i<4;i++){
                cout<<IR[i];
                
            }
            cout<<"\n";
            cout<<"\nIC: "<<IC<<"\n";
            IC++;

            //If LR, Load the register
            if(IR[0] == 'L' && IR[1]=='R'){
                cout << "Loading register\n";
                for(int i=0;i<4;i++){
                    R[i] = Memory[(IR[2] - '0')*10 + (IR[3] - '0')][i];
                }
            }
            //If SR, store from register to Memory
            else if(IR[0] == 'S' && IR[1]=='R'){
                for(int i=0;i<4;i++){
                    cout << "Register to Memory";
                    Memory[(IR[2] - '0')*10 + (IR[3] - '0')][i] = R[i];
                }
            }
            
            else if(IR[0]=='C' && IR[1]=='R'){
                int temp = 0;
                for(int i=0;i<4;i++){
                    if(Memory[(IR[2] - '0')*10 + (IR[3] - '0')][i] == R[i]){
                        temp++;
                    }
                }
                if(temp==4){
                    cout<<"\nTemp = 4\n";
                    toggle = true;
                }
                else{
                    cout<<"\nTemp != 4\n";
                    toggle = false;}
            }

            else if(IR[0]=='B' && IR[1]=='T'){
                if(toggle){
                    IC = (IR[2] - '0')*10 + (IR[3] - '0');
                }
            }

            else if(IR[0]=='G' && IR[1]=='D'){
                SI = 1;
                masterMode(); 
            }

            else if(IR[0]=='P' && IR[1]=='D'){
                SI = 2;
                masterMode(); 
            }

            else if(IR[0]=='H'){
                SI = 3;
                masterMode();
                return; 
            }

    }

    }

    void masterMode(){
        cout<<"\nSI: "<<SI<<endl;
        switch (SI)
        {
            case 1:
                read();
                break;

            case 2:
                write();
                break;
            
            case 3:
                halt();
                break;
                
        }
        SI = 0;
    }

    void read(){
        cout<<"Read func called\n";
        IR[3] = '0';
        string cc2;
        getline(newfile,cc2);
        int len = cc2.size();
        for(int i = 0;i<len;i++){
            buffer[i] = cc2[i];
        }
        int buff = 0, mem_ptr = (IR[2]-'0') *10;
        while(buff<40 && buffer[buff]!='@'){
            for(int i=0;i<4;i++){
                Memory[mem_ptr][i] = buffer[buff];
                buff++;
            }
            mem_ptr++;
        }
        clear_buffer();
        //executeUser();

    }
    
    void write(){
        cout<<"Write func called\n";
        op.open("Output_1.txt", std::fstream::app);
        
        IR[3] = '0';
        for (int i = (IR[2] - '0')*10; i < (IR[2] - '0' + 1)*10; i++){
            for (int j = 0; j < 4; j++){
                if(Memory[i][j]!='@'){
                    op << Memory[i][j];
                }
            }
        }
        op<<"\n";
        op.close();

    }

    void halt(){
        IC = 100;
        op.open("Output_1.txt", std::fstream::app);
        cout<<"halt called\n\n";
        op<<"\n\n";
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
