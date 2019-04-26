//
//  main.cpp
//  StaticDataManagerGenerator
//
//  Created by 민경효 on 25/04/2019.
//  Copyright © 2019 cro. All rights reserved.
//

//#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unistd.h>
#include <vector>
#include <map>
#include <set>
#include <deque>

#define DEBUG 1
#define LINES_TO_IGNORE 11

//using namespace std;

std::regex comment("\\s*#.*");                                                  // # 시즌 종료 후 정산하는데 쓸 여유 시간
std::regex import("\\s*using\\s+(\\w+)\\s*=\\s*import\\s+([\"./\\w]+);\\s*");   // using Action = import "achievements.capnp".Action;
std::regex import2("\\s*using\\s+import\\s+([\"./\\w]+);\\s*");                 // using import "events.capnp".MessageReward;
std::regex struct_root("\\s*struct\\s+(\\w+)\\s+\\$Data.root\\s*\\{\\s*");      // struct MysteryMazeMode $Data.root {
std::regex struct_usual("\\s*struct\\s+(\\w+)\\s*\\{\\s*");                     // struct Season {
std::regex elem_key("\\s*(\\w+)\\s+@\\d+\\s*:\\s*(\\w+)\\s+\\$GB.key;\\s*");    // id                       @0 :Int16 $GB.key;
std::regex elem("\\s*(\\w+)\\s+@\\d+\\s*:\\s*(\\w+);\\s*");                     // startTime                @1 :Int64;
std::regex elem_list("\\s*(\\w+)\\s+@\\d+\\s*:\\s*List\\((\\w+)\\);\\s*");      // mazeTables               @2 :List(MazeTable);
std::regex serverOnly(".*\\$Data.serverOnly;\\s*");                             // itemHardLimits           @3 :List(ItemData) $Data.serverOnly;
std::regex closingBrace("\\s*\\}\\s*");                                         // }

std::map<std::string, std::string> baseTypes;       // 기본 타입 (int, string)
std::map<std::string, std::string> definedTypes;    // 특수 처리가 필요한 타입
std::map<std::string, std::string> enumTypes;       // enum 타입
std::set<std::string> typesWithKey;                 // GB.Key가 있는 타입

void insertBaseType()
{
    
}
void readStruct(std::ifstream& input, std::deque<std::string>& parents, std::string name, int& linenum)
{
    parents.push_back(name);
    
    std::string line;
    while(getline(input, line))
    {
        linenum++;
        if(linenum <= LINES_TO_IGNORE)
            continue;
        
        if(line.compare("") == 0)
            continue;
        
        std::smatch matches;
        if(std::regex_search(line, matches, comment)) //주석
        {
            continue;
        }
        else if(std::regex_search(line, matches, struct_usual)) //struct_usual
        {
            
            
            if(DEBUG == 2)
            {
                std::cout << 1 << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
        }
        else if(std::regex_search(line, matches, import2)) //import2
        {
            if(DEBUG == 2)
            {
                std::cout << 2 << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
        }
        else if(std::regex_search(line, matches, struct_root)) //struct_root
        {
            if(DEBUG == 2)
            {
                std::cout << "struct_root" << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
        }
        else if(std::regex_search(line, matches, struct_usual)) //struct
        {
            if(DEBUG == 2)
            {
                std::cout << "struct" << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
        }
        else
        {
            std::cout << "ERROR: Unknown pattern found, failed to parse line " << linenum << ".\n";
            std::cout << "The line at fault: \n" << line << "\n\n";
            return;
        }
    }
    
    parents.pop_back();
}

int main(int argc, const char * argv[])
{
    if(argc < 1)
    {
        std::cout << "ERROR: No file name given.";
        return 0;
    }
    std::ifstream input(argv[1]);
    
    if(DEBUG)
    {
        char path[1024];
        getcwd(path, sizeof(path));
        std::cout << "Current path is " << path << '\n';
        std::cout << "File name: " << argv[1] << std::endl;
    }
    
    std::string line;
    int linenum = 0;
    
    while(getline(input, line))
    {
        linenum++;
        if(linenum <= LINES_TO_IGNORE)
            continue;
        
        if(line.compare("") == 0)
            continue;
        
        std::smatch matches;
        if(std::regex_search(line, matches, comment)) //주석
        {
            continue;
        }
        else if(std::regex_search(line, matches, import)) //import
        {
            if(DEBUG == 2)
            {
                std::cout << 1 << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
        }
        else if(std::regex_search(line, matches, import2)) //import2
        {
            if(DEBUG == 2)
            {
                std::cout << 2 << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
        }
        else if(std::regex_search(line, matches, struct_root)) //struct_root
        {
            if(DEBUG == 2)
            {
                std::cout << "struct_root" << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
            std::deque<std::string> parents = std::deque<std::string>();
            readStruct(input, parents, matches[1].str(), linenum);
        }
        else if(std::regex_search(line, matches, struct_usual)) //struct
        {
            if(DEBUG == 2)
            {
                std::cout << "struct" << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
            std::deque<std::string> parents = std::deque<std::string>();
            readStruct(input, parents, matches[1].str(), linenum);
        }
        else
        {
            std::cout << "ERROR: Unknown pattern found, failed to parse line " << linenum << ".\n";
            std::cout << "The line at fault: \n" << line << "\n\n";
            return 0;
        }
        
        
        /*
        std::vector<std::string> tokens;
        std::regex reg("\\s+");
        std::sregex_token_iterator iter(line.begin(), line.end(), reg, -1);
        std::sregex_token_iterator end;
        std::vector<std::string> vec(iter, end);
        
        for(auto token : vec)
        {
            tokens.push_back(token);
            if(DEBUG)
                std::cout << token << std::endl;
        }
         */
    }
    
    return 0;
}

