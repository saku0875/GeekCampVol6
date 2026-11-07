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

int TaskCard::newWriteTaskDB(std::string taskContent){
    int csvCount = 0;
    Task Task_buf;
    string str_buf;
    string str_conma_buf;
    string input_csv_file_path  = "DB/Test.csv";
    string output_csv_file_path = "DB/Test.csv";
    string Today;
    
    ifstream ifs_csv_file(input_csv_file_path);
    ofstream ofs_csv_file(output_csv_file_path,std::ios::app);

    Task_buf.id = 0;
    Task_buf.date = 0;
    Task_buf.TaskContent = "";
    Task_buf.flags = false;
    while (getline(ifs_csv_file,str_buf)){
        istringstream i_stream(str_buf);
        while(getline(i_stream, str_conma_buf, ',')){
            if(csvCount == 0){
                std::cout << str_conma_buf << std::endl;
                Task_buf.id = std::stoi(str_conma_buf);
                csvCount++;
            }else{
                break;
            }
        }
        csvCount = 0;
    }
    Task_buf.id++;
    Today = GetCurrentDay();
    ofs_csv_file << Task_buf.id << ',' << Today << ','<< taskContent << ','<< false << std::endl;
    return 0;
}



int TaskCard::flagChange(int id){
    int csvCount = 0;
    Task Task_buf;
    string str_buf;
    string str_conma_buf;
    string output_csv_file_path = "DB/Test.csv";

    string output_csv_content;
    bool changed = false;
    ifstream ifs_csv_file(output_csv_file_path);
    if (!ifs_csv_file.is_open()){
        return -1;
    }

    while (getline(ifs_csv_file,str_buf)){
        string updated_line = str_buf;
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
                if (id == Task_buf.id){
                    updated_line = str_buf.substr(0, str_buf.rfind(',') + 1)
                        + (Task_buf.flags ? "0" : "1");
                    changed = true;
                }
            }
        }
        output_csv_content += updated_line;
        if (!ifs_csv_file.eof()){
            output_csv_content += '\n';
        }
        csvCount = 0;
        Task_buf.id = 0;
        Task_buf.date = 0;
        Task_buf.TaskContent = "";
        Task_buf.flags = false;

    }

    if (ifs_csv_file.bad() || !ifs_csv_file.eof()){
        return -1;
    }
    ifs_csv_file.close();
    if (!changed){
        return 0;
    }

    ofstream ofs_csv_file(output_csv_file_path);
    if (!ofs_csv_file.is_open()){
        return -1;
    }
    ofs_csv_file << output_csv_content;
    ofs_csv_file.close();
    return ofs_csv_file.fail() ? -1 : 0;
}
    