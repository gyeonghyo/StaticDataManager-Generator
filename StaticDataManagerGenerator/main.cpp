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

#define MY_DEBUG 3
#define LINES_TO_IGNORE 11

//using namespace std;

std::regex comment("\\s*#.*");                                                      // # 시즌 종료 후 정산하는데 쓸 여유 시간
std::regex import("\\s*using\\s+(\\w+)\\s*=\\s*import\\s+([\"./\\w]+)\\s*;\\s*");   // using Action = import "achievements.capnp".Action;
std::regex import2("\\s*using\\s+import\\s+([\"./\\w]+);\\s*");                     // using import "events.capnp".MessageReward;
std::regex struct_root("\\s*struct\\s+(\\w+)\\s+\\$Data.root\\s*\\{\\s*");          // struct MysteryMazeMode $Data.root {
std::regex struct_usual("\\s*struct\\s+(\\w+)\\s*\\{\\s*");                         // struct Season {
std::regex elem_key("\\s*(\\w+)\\s+@\\d+\\s*:\\s*(\\w+)\\s+\\$GB.key\\s*;\\s*");    // id                       @0 :Int16 $GB.key;
std::regex elem("\\s*(\\w+)\\s+@\\d+\\s*:\\s*(\\w+);\\s*");                         // startTime                @1 :Int64;
std::regex elem_list("\\s*(\\w+)\\s+@\\d+\\s*:\\s*List\\((\\w+)\\)\\s*;\\s*");      // mazeTables               @2 :List(MazeTable);
std::regex serverOnly(".*\\$Data.serverOnly;\\s*");                                 // itemHardLimits           @3 :List(ItemData) $Data.serverOnly;
std::regex closingBrace("\\s*\\}\\s*");                                             // }
std::regex enum_usual("\\s*enum\\s+(\\w+)\\s*\\{\\s*");                             // enum Source {
std::regex elem_enum("\\s*(\\w+)\\s+@\\d+\\s*");                                    // none @0;
std::regex whiteSpace("\\s*");                                                      //

/* key - capnpName, value - cppName */
std::map<std::string, std::string> convertName_baseTypes;           // 기본 타입 (int, string)
/* key - scopeName, value - cppName */
std::map<std::string, std::string> convertName_enumTypes;           // enum 타입
//std::map<std::string, std::string> convertName_structTypes;        // struct 타입

/* key - cppName, value - cppName */
std::map<std::string, std::string> keyNames;                     // GB.Key가 있는 타입
std::map<std::string, std::deque<std::pair<std::string, std::string>>> members;   //pair<capnpName, cppType>
std::map<std::string, std::deque<std::pair<std::string, std::string>>> listTypeMembers;   //pair<capnpName, cppType>

bool isBaseType(std::string typeName)
{
    return convertName_baseTypes.count(typeName) != 0;
}

bool isEnumType(std::string typeName)
{
    return convertName_baseTypes.count(typeName) != 0;
}


void insertBaseTypes()
{
    convertName_baseTypes["Text"] = "std::string";
    convertName_baseTypes["Int64"] = "int64_t";
    convertName_baseTypes["Int32"] = "int32_t";
    convertName_baseTypes["Int16"] = "int16_t";
    convertName_baseTypes["Int8"] = "int8_t";
}

std::string getKey(std::deque<std::string>& parents, std::string name = "")
{
    if(name.compare("") != 0)
        parents.push_back(name);
    
    std::string ret;
    auto iter = parents.begin();
    if(iter != parents.end())
    {
        ret += *iter;
        iter++;
        for(; iter != parents.end(); iter++)
        {
            ret += '_';
            ret += *iter;
        }
    }
    
    if(name.compare("") != 0)
        parents.pop_back();
    return ret;
}

void readEnum(std::ifstream& input, std::deque<std::string>& parents, std::string name, int& linenum)
{
    
}

void readStruct(std::ifstream& input, std::deque<std::string>& parents, std::string name, int& linenum)
{
    std::string parentDir = getKey(parents);
    std::string dir = getKey(parents, name);
    
    parents.push_back(name);
    
    if(MY_DEBUG == 2) std::cout << "pDir: " << parentDir << '\n';
    if(MY_DEBUG == 2) std::cout << "dir: " << dir << '\n';
    
    std::string line;
    while(getline(input, line))
    {
        linenum++;
        std::smatch matches;
        
        if(std::regex_match(line, matches, whiteSpace) ||
           std::regex_match(line, matches, comment))
        {
            continue;
        }
        else if(std::regex_match(line, matches, struct_usual))
        {
            readStruct(input, parents, matches[1].str(), linenum);
            if(MY_DEBUG == 2) std::cout << "struct_usual" << '\n';
        }
        else if(std::regex_match(line, matches, elem_key) ||
                std::regex_match(line, matches, elem))
        {
            std::string cppKeyType = "";
            std::string keyType = matches[2].str();
            if(isBaseType(keyType))
            {
                cppKeyType = convertName_baseTypes[keyType];
            }
            else
            {
                if(convertName_enumTypes.count(dir + '_' + keyType) != 0)
                {
                    cppKeyType = convertName_enumTypes[parentDir + '_' + keyType];
                }
                else if(convertName_enumTypes.count(parentDir + '_' + keyType) != 0)
                {
                    cppKeyType = convertName_enumTypes[dir + '_' + keyType];
                }
                else
                {
                    std::cout << "ERROR: Unknown type '" << keyType << "' at line " << linenum << ".\n";
                }
            }
            
            members[dir].push_back(make_pair(matches[1].str(), cppKeyType));
            if(std::regex_match(line, matches, elem_key))
               keyNames[dir] = cppKeyType;
            
            
            if(MY_DEBUG == 2) std::cout << "elem" << '\n';
            if(MY_DEBUG == 3)
            {
                std::cout << members[dir].begin()->first << '\n';
                std::cout << members[dir].begin()->second << '\n';
            }
        }
        else if(std::regex_match(line, matches, elem_list))
        {
            if(MY_DEBUG == 2) std::cout << "elem_list" << '\n';
        }
        else if(std::regex_match(line, matches, serverOnly))
        {
            if(MY_DEBUG == 2) std::cout << "serverOnly" << '\n';
        }
        else if(std::regex_match(line, matches, closingBrace))
        {
            if(MY_DEBUG == 2) std::cout << "closingBrace" << '\n';
            parents.pop_back();
            return;
        }
        else
        {
            std::cout << "ERROR: Unknown pattern found, failed to parse line " << linenum << ".\n";
            std::cout << "The line at fault: \n" << line << "\n\n";
            return;
        }
    }
    
    parents.pop_back();
    return;
}

int main(int argc, const char * argv[])
{
    insertBaseTypes();
    
    if(argc < 1)
    {
        std::cout << "ERROR: No file name given.";
        return 0;
    }
    std::ifstream input(argv[1]);
    
    if(MY_DEBUG)
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
        std::smatch matches;
        if(linenum <= LINES_TO_IGNORE               ||
           std::regex_match(line, matches, comment) ||
           std::regex_match(line, matches, whiteSpace)) //주석
        {
            continue;
        }
        else if(std::regex_match(line, matches, import)) //import
        {
            if(MY_DEBUG == 2)
            {
                std::cout << 1 << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
        }
        else if(std::regex_match(line, matches, import2)) //import2
        {
            if(MY_DEBUG == 2)
            {
                std::cout << 2 << '\n';
                for (size_t i = 0; i < matches.size(); ++i) {
                    std::cout << i << ": '" << matches[i].str() << "'\n";
                }
            }
        }
        else if(std::regex_match(line, matches, struct_root) ||
                std::regex_match(line, matches, struct_usual)) //struct_root
        {
            if(MY_DEBUG == 2)
            {
                std::cout << "struct_root" << '\n';
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
    }
    
    input = std::ifstream(argv[1]);
    linenum = 0;
    
    while(getline(input, line))
    {
        linenum++;
        std::smatch matches;
        if(linenum <= LINES_TO_IGNORE                   ||
           std::regex_match(line, matches, whiteSpace)  ||
           std::regex_match(line, matches, comment)     ||
           std::regex_match(line, matches, import)      ||
           std::regex_match(line, matches, import2))
        {
            continue;
        }
        else if(std::regex_match(line, matches, struct_root) ||
                std::regex_match(line, matches, struct_usual)) //struct_root
        {
            if(MY_DEBUG == 2)
            {
                std::cout << "struct_root" << '\n';
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
    }
    return 0;
}

