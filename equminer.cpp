#include "equminer.h"
vector<Input *> input_vec;
vector<Output *> output_vec;
vector<Stdin_format *> std_vec;
vector<Equminer *> equ_vec;

string input_path; //path of INPUT directory
string dir_path;   //path of current directory
string dir_name;   //name of current directory
string file_apath; //absolute path of current file
string file_rpath; //relative path of current file
string file_name;  //name of current file
void generate_input()
{
    const char *dir_path_name = dir_path.c_str();
    DIR *curdir_dir = opendir(dir_path_name);
    if (curdir_dir == NULL)
    {
        cout << "Fail to open current file directory!" << endl;
    }
    else
    {
        dirent *curfile;
        //Traverse files of CURRENT directory
        while ((curfile = readdir(curdir_dir)) != NULL)
        {
            file_name = curfile->d_name;
            if (file_name != "." && file_name != ".." && file_name != "stdin_format.txt" && file_name != "input.txt" && file_name != "output.txt" && file_name != "output1.txt" && file_name != "output2.txt" && file_name != "a.out" && file_name != "b.out")
            {
                file_apath = dir_path + "/" + file_name;
                file_rpath = "input/" + dir_name + "/" + file_name;
                Input *curInput = new Input(file_name, file_apath, file_rpath, dir_path);
                input_vec.push_back(curInput);
                //cout<<file_name<<' '<<curfile_absolute_path<<' '<<curfile_relative_path<<endl;
            }
        }
        closedir(curdir_dir);
    }
}

void generate_stdin_format()
{
    file_apath = dir_path + "/stdin_format.txt";
    ifstream in_file(file_apath);

    if (!in_file)
    {
        cout << "Fail to open stdin_format.txt!" << endl;
    }
    else
    {
        char buffer[51];
        while (in_file >> buffer)
        {
            int t, l, r;
            switch (buffer[0])
            {
            case 'i':
            {
                t = 1;
                if (buffer[3] == '(')
                {
                    string left_num = "", right_num = "";
                    for (int i = 4; i <= 50; ++i)
                    {
                        if (buffer[i] == ',')
                            break;
                        else
                            left_num += buffer[i];
                    }
                    l = stoi(left_num);
                    for (int i = 4 + left_num.size() + 1; i <= 50; ++i)
                    {
                        if (buffer[i] == ')')
                            break;
                        else
                            right_num += buffer[i];
                    }
                    r = stoi(right_num);
                }
                else
                    cout << "Error in stdin_format.txt!" << endl;

                break;
            }
            case 'c':
            {
                t = 2;
                l = 1;
                r = 1;
                break;
            }
            case 's':
            {
                t = 2;
                if (buffer[6] == '(')
                {
                    string left_num = "", right_num = "";
                    for (int i = 7; i <= 50; ++i)
                    {
                        if (buffer[i] == ',')
                            break;
                        else
                            left_num += buffer[i];
                    }

                    l = stoi(left_num);
                    for (int i = 7 + left_num.size() + 1; i <= 50; ++i)
                    {
                        if (buffer[i] == ')')
                            break;
                        else
                            right_num += buffer[i];
                    }
                    r = stoi(right_num);
                }
                else
                    cout << "Error in stdin_format.txt!" << endl;

                break;
            }
            default:
            {
                cout << "Error in stdin_format.txt!" << endl;
                break;
            }
            }
            if (l <= r)
            {
                Stdin_format *sf = new Stdin_format(t, l, r);
                std_vec.push_back(sf);
            }
            else
            {
                cout << "Error in stdin_format.txt!" << endl;
            }
        }
    }
    in_file.close();
}

Stdin_format::Stdin_format() {}
Stdin_format::Stdin_format(int t, int l, int r) : type(t), left(l), right(r) {}
int Stdin_format::get_type() { return type; }
int Stdin_format::get_left() { return left; }
int Stdin_format::get_right() { return right; }

Equminer::Equminer() {}
Equminer::Equminer(Input *i1, Input *i2, Output *o, vector<Stdin_format *> &st) : input1(i1), input2(i2), output(o), stf(st) {}

string Equminer::random_case(int n) //Generate random test samples
{
    int t = stf[n]->get_type();
    int rand_num = stf[n]->get_left() + rand() % (stf[n]->get_right() - stf[n]->get_left() + 1);
    if (t == 1)
    { //int
        return to_string(rand_num);
    }
    else if (t == 2)
    { //char or string
        string str = "";
        char rand_char;
        for (int i = 0; i < rand_num; ++i)
        {
            rand_char = 'a' + rand() % 26;
            str += rand_char;
        }
        return str;
    }
}
void Equminer::case_compare() //Compare using test samples
{
    char creator1[1001] = "g++ -w -o a.out ";
    strcat(creator1, input1->get_apath().c_str());
    system(creator1);

    char creator2[1001] = "g++ -w -o b.out ";
    strcat(creator2, input2->get_apath().c_str());
    system(creator2);

    
    for (int i = 0; i < 10; ++i)
    { //10 times
        string inputfile_path = input1->get_dpath() + "/input.txt";
        string outputfile_path1 = input1->get_dpath() + "/output1.txt";
        string outputfile_path2 = input2->get_dpath() + "/output2.txt";

        ofstream out_file;
        out_file.open(inputfile_path);

        //a group of randomly generated samples
        for (int j = 0; j < stf.size(); ++j)
        {
            out_file << random_case(j) << ' ';
            //cout<<random_case(j)<<' ';
        }
       
        out_file.close();

        
        char runner1[1001] = "./a.out <";
        strcat(runner1, inputfile_path.c_str());
        strcat(runner1, " >");
        strcat(runner1, outputfile_path1.c_str());
        int r1 = system(runner1);

        char runner2[1001] = "./b.out <";
        strcat(runner2, inputfile_path.c_str());
        strcat(runner2, " >");
        strcat(runner2, outputfile_path2.c_str());
        int r2 = system(runner2);

        
        if ((r1 != 0 && r2 == 0) || (r2 != 0 && r1 == 0))
        {
            output->set_equal(0);
            return;
        }
        if (r1 != 0 && r2 != 0)
        {
            continue;
        }

        //read output of each file and compare
        fstream in_file1;
        vector<string> res1;
        in_file1.open(outputfile_path1);
        if (!in_file1)
        {
            cout << "Fail to open output1.txt!" << endl;
        }
        else
        {
            char buffer[51];
            while (in_file1 >> buffer)
            {
                string buf = buffer;
                res1.push_back(buf);
            }
        }
        in_file1.close();

        fstream in_file2;
        vector<string> res2;
        in_file2.open(outputfile_path2, ios::in);
        if (!in_file2)
        {
            cout << "Fail to open output2.txt!" << endl;
        }
        else
        {
            char buffer[51];
            while (in_file2 >> buffer)
            {
                string buf = buffer;
                res2.push_back(buf);
            }
        }
        in_file2.close();

        //compare
        if (res1.size() != res2.size())
        {
            output->set_equal(0);
            return;
        }
        else
        {
            for (int j = 0; j < res1.size(); ++j)
            {
                if (res1[j] != res2[j])
                {
                    output->set_equal(0);
                    return;
                }
            }
        }
    }
    //same output for ten times,then equal
    output->set_equal(1);
    //cout<<endl;
}
void output_csv(){
    //equal
    ofstream out_file_equal;
    out_file_equal.open("equal.csv");
    out_file_equal<<"file1"<<','<<"file2"<<endl;

    //inequal
    ofstream out_file_inequal;
    out_file_inequal.open("inequal.csv");
    out_file_inequal<<"file1"<<','<<"file2"<<endl;
    
    for(int i = 0;i < output_vec.size();i++)
    {
        if(output_vec[i]->get_equal() == 0){
            //inequal
            out_file_inequal<<output_vec[i]->get_input1()->get_rpath()<<','<<output_vec[i]->get_input2()->get_rpath()<<endl;
        }
        else if(output_vec[i]->get_equal() == 1){
            //equal
            out_file_equal<<output_vec[i]->get_input1()->get_rpath()<<','<<output_vec[i]->get_input2()->get_rpath()<<endl;
        }
    }

    out_file_equal.close();
    out_file_inequal.close();
}


int main()
{
    //read path of INPUT
    input_path = "/home/njucs/桌面/input";
    const char *input_path_name = input_path.c_str();
    DIR *input_dir = opendir(input_path_name);
    if (input_dir == NULL)
    {
        cout << "Fail to open INPUT file directory!" << endl;
    }
    else
    {
        dirent *curdir;
        //Traverse subfolders of INPUT
        while ((curdir = readdir(input_dir)) != NULL)
        {
            dir_name = curdir->d_name;
            if (dir_name != ".vscode" && dir_name != "." && dir_name != "..")
            {
                //generate Input and Stdin_format objects
                dir_path = input_path + "/" + dir_name;
                //cout<<curdir_path<<endl;
                generate_input();
                generate_stdin_format();

                //generate Equminer and Output objects
                //then compare each Input object, get results
                //equminer_test();
                for (int i = 0; i < input_vec.size() - 1; ++i)
                {
                    for (int j = i + 1; j < input_vec.size(); ++j)
                    {
                        //generate Output objects
                        Output *op = new Output(input_vec[i], input_vec[j]);
                        output_vec.push_back(op);

                        //generate Equminer objects
                        Equminer *eq = new Equminer(input_vec[i], input_vec[j], op, std_vec);
                        equ_vec.push_back(eq);

                        //random test
                        eq->case_compare();
                    }
                }
                //empty input and stdin vector, for next directory
                input_vec.clear();
                std_vec.clear();
            }
        }
        closedir(input_dir);
    }

    //output results
    output_csv();
    return 0;
}