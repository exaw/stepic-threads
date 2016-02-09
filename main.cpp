#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

vector<string> ParseInput()
{
    vector<string> res;
    while (1)
    {
        char line[1024];
        cin.getline(line, 1024, '|');
        string sline(line);
        if (sline.size() == 0)
        {
            break;
        }

        res.push_back(sline);
    }

    return res;
}

class Command
{
public:
    Command(string command)
    {
        istringstream istr(command);
        string base;
        istr >> base;

        command_ = new char[base.size() + 1];
        command_[base.size()] = 0;
        std::copy(base.cbegin(), base.cend(), command_);

        vector<string> params;
        params.push_back(string(command_));

        while (!istr.eof())
        {
            string param;
            istr >> param;
            params.push_back(param);
        }

        nbArgv_ = params.size();
        argv_ = new char *[nbArgv_ + 1];
        argv_[nbArgv_] = nullptr;

        for (int i = 0; i < nbArgv_; i++)
        {
            string param = params[i];
            argv_[i] = new char[param.size() + 1];
            argv_[i][param.size()] = 0;
            std::copy(param.cbegin(), param.cend(), argv_[i]);
        }
    }

    char *GetCommandPtr() const
    {
        return command_;
    }

    int GetNbArgv() const
    {
        return nbArgv_;
    }

    char **GetArgvPtr() const
    {
        return argv_;
    }

private:
    char **argv_ = 0;
    int nbArgv_ = 0;
    char *command_ = 0;
};

void exe(vector<string>::iterator it, vector<string>::iterator begin, vector<string>::iterator end, int in)
{
    if (it == end)
    {
        return;
    }

    int fd[2];
    pipe(fd);
    if (!fork())
    {
        if (it != end - 1)
        {
            close(STDOUT_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
        }
        else
        {
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
            //int fid = open("/home/ta/tmp/result.out", O_CREAT | O_WRONLY, mode);
            int fid = open("/home/box/result.out", O_CREAT | O_WRONLY, mode);
            close(STDOUT_FILENO);
            dup2(fid, STDOUT_FILENO);
            close(fid);
            close(fd[0]);
            close(fd[1]);
        }

        Command command(*it);
        cout<<command.GetCommandPtr();
        execvp(command.GetCommandPtr(), command.GetArgvPtr());
    }
    else
    {
        close(STDIN_FILENO);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);

        exe(it + 1, begin, end, 0);
    }

}

int main()
{
   // auto input = vector<string> {"echo 1"};
    auto input = ParseInput();

    for (auto command : input)
    {
       // cout << "Command: " << command << endl;
    }

    exe ( input.begin(), input.begin(), input.end(), 0);
    return 0;
    //Command command("echo 1 123");
    //execvp(command.GetCommandPtr(), command.GetArgvPtr());

    return 0;
}