#include "TaskCard.hpp"

TaskCard::TaskCard(){
    
}
TaskCard::~TaskCard(){
    
}

Task TaskCard::ReadTaskDB(int id,bool flags){
    int csvCount = 0;
    Task Task_buf;
    string str_buf;
    string str_conma_buf;
    string input_csv_file_path  = "DB/Test.csv";
    
    ifstream ifs_csv_file(input_csv_file_path);

    Task_buf.id = 0;
    Task_buf.date = 0;
    Task_buf.TaskContent = "";
    Task_buf.flags = false;
    while (getline(ifs_csv_file,str_buf)){
        istringstream i_stream(str_buf);
        while(getline(i_stream, str_conma_buf, ',')){
            if(csvCount == 0){
                Task_buf.id = std::stoi(str_conma_buf);
                csvCount++;
            }else if(csvCount == 1){
                Task_buf.date = std::stoi(str_conma_buf);
                csvCount++;
            }else if(csvCount == 2){
                Task_buf.TaskContent = str_conma_buf;
                csvCount++;
            }else if(csvCount == 3){
                Task_buf.flags = stringToBool(str_conma_buf);
                csvCount++;
            }else{
                break;
            }
        }
        if (Task_buf.id == id && Task_buf.flags == flags){
                return Task_buf;
        }else{
            csvCount = 0;
            Task_buf.id = 0;
            Task_buf.date = 0;
            Task_buf.TaskContent = "";
            Task_buf.flags = false;
        }

    }
    return Task_buf;
}