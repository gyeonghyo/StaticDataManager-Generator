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

#include <sstream>
#include <iomanip>
#include <ctime>
#include <ctype.h>

#define MY_DEBUG 4
#define LINES_TO_IGNORE 11
#define TAB (std::string)"    "
#define WARNING (std::string)"PLEASE CHECK IMPORTED TYPE"

//using namespace std;

std::regex comment("\\s*#.*");                                                          // # 시즌 종료 후 정산하는데 쓸 여유 시간
std::regex import("\\s*using\\s+(\\w+)\\s*=\\s*import\\s+([\"./\\w]+)\\s*;\\s*");       // using Action = import "achievements.capnp".Action;
std::regex import2("\\s*using\\s+import\\s+([\"./\\w]+);\\s*");                         // using import "events.capnp".MessageReward;
std::regex struct_root("\\s*struct\\s+(\\w+)\\s+\\$Data.root\\s*\\{\\s*");              // struct MysteryMazeMode $Data.root {
std::regex struct_usual("\\s*struct\\s+(\\w+)\\s*\\{\\s*");                             // struct Season {
std::regex elem_key("\\s*(\\w+)\\s+@\\d+\\s*:\\s*([\\.\\w]+)\\s+\\$GB.key\\s*;\\s*");   // id                       @0 :Int16 $GB.key;
std::regex elem("\\s*(\\w+)\\s+@\\d+\\s*:\\s*([\\.\\w]+);\\s*");                        // startTime                @1 :Int64;
std::regex elem_list("\\s*(\\w+)\\s+@\\d+\\s*:\\s*List\\(([\\.\\w]+)\\)\\s*;\\s*");     // mazeTables               @2 :List(MazeTable);
std::regex serverOnly(".*\\$Data.serverOnly;\\s*");                                     // itemHardLimits           @3 :List(ItemData) $Data.serverOnly;
std::regex closingBrace("\\s*\\}\\s*");                                                 // }
std::regex enum_usual("\\s*enum\\s+(\\w+)\\s*\\{\\s*");                                 // enum Source {
std::regex elem_enum("\\s*(\\w+)\\s+@\\d+\\s*;\\s*");                                   // none @0;
std::regex whiteSpace("\\s*");                                                          //

/* key - capnpName, value - cppName */
std::map<std::string, std::string> convertName_baseTypes;           // 기본 타입 (int, string)
/* key - capnpName, value - cppName */
std::map<std::string, std::string> convertName_importedTypes;       // import 된 타입
/* key - scopeName, value - cppName */
std::map<std::string, std::string> convertName_enumTypes;           // enum 타입
/* elem - scopeName == cppName */
std::set<std::string> existing_structTypes;                         // struct 타입

/* elem - cppName */
std::set<std::string> existing_importedTypes;                       // .cpp 에 생성된 imported 타입

/* key - cppName, value - cppType */
std::map<std::string, std::string> keyTypes;                        // GB.Key
/* key - cppName, value - capnpName */
std::map<std::string, std::string> keyNames;
std::map<std::string, std::deque<std::pair<std::string, std::string>>> members;   //pair<capnpName, cppType>
std::map<std::string, std::deque<std::pair<std::string, std::string>>> listTypeMembers;   //pair<capnpName, cppType>

/* elem - cppType*/
std::set<std::string> baseTypes;

std::string rootName;

//util
std::string low(std::string orig);
std::string up(std::string orig);
std::string replaceAll(std::string str, const std::string& from, const std::string& to);

//capnp baseType 인가?
bool isBaseType(std::string typeName)
{
    return convertName_baseTypes.count(typeName) != 0;
}

//cpp baseType 인가?
bool isCppBaseType(std::string cppTypeName)
{
	return baseTypes.count(cppTypeName) != 0;
}

//cpp structType 인가?
bool isCppStructType(std::string cppTypeName)
{
	return existing_structTypes.count(cppTypeName) != 0;
}

//import 된 것인가?
bool isImportedType(std::string cppType)
{
	return existing_importedTypes.count(cppType) != 0;
}

//key 가 있는가?
bool hasKey(std::string cppTypeName)
{
	return keyTypes.count(cppTypeName) != 0;
}

void insertBaseTypes()
{
    convertName_baseTypes["Text"] = "std::string";
    convertName_baseTypes["Bool"] = "bool";
    convertName_baseTypes["Float32"] = "float";
    convertName_baseTypes["Int64"] = "int64_t";
    convertName_baseTypes["Int32"] = "int32_t";
    convertName_baseTypes["Int16"] = "int16_t";
    convertName_baseTypes["Int8"] = "int8_t";
	for (const auto& pa : convertName_baseTypes)
	{
		baseTypes.insert(pa.second);
	}
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

std::string getCppNameSpace(std::deque<std::string>& parents)
{
    std::string ret = "gb::capnp::gamedata";
    for(auto st : parents)
    {
        ret += "::";
        ret += st;
    }
    return ret;
}

//capnp reader 의 타입 반환
std::string getReader(std::string cppType)
{
	auto ret = replaceAll(cppType, "_", "::");
	ret = "gb::capnp::gamedata::" + ret + "::Reader";
	return ret;
}

//단순히 enum을 convertName_enumTypes 에 저장
void readEnum(std::ifstream& input, std::deque<std::string>& parents, std::string name, int& linenum)
{
    std::string dir = getKey(parents, name);
    parents.push_back(name);
    
    //실행 목적: enum의 위치와 cpp 이름을 파악하기 위해
    convertName_enumTypes[dir] = getCppNameSpace(parents);
    
    if(MY_DEBUG == 2) std::cout << "enum ns: " << getCppNameSpace(parents) << '\n';
    
    std::string line;
    while(getline(input, line))
    {
        linenum++;
        std::smatch matches;
        
        if(std::regex_match(line, matches, whiteSpace) ||
           std::regex_match(line, matches, comment)    ||
           std::regex_match(line, matches, elem_enum))
        {
            continue;
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
    
    std::cout << "ERROR: Unexpected end of file at line " << linenum << ".\n";
    return;
}

//2회차: 파일을 읽으며 멤버 변수들을 저장
void readElem(std::ifstream& input, std::deque<std::string>& parents, std::string name, int& linenum)
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
           std::regex_match(line, matches, comment)    ||
           std::regex_match(line, matches, serverOnly))
        {
            continue;
        }
        else if(std::regex_match(line, matches, struct_usual))
        {
            readElem(input, parents, matches[1].str(), linenum);
            if(MY_DEBUG == 2) std::cout << "struct_usual" << '\n';
        }
        else if(std::regex_match(line, matches, enum_usual))
        {
            readEnum(input, parents, matches[1].str(), linenum);
        }
        else if(std::regex_match(line, matches, elem_key) ||
                std::regex_match(line, matches, elem)     ||
                std::regex_match(line, matches, elem_list))
        {
            //capnp의 변수명을 cpp의 변수명으로 변환 후 멤버 변수 리스트에 저장
            std::string cppKeyType = "";
            std::string keyType = matches[2].str();
            if(isBaseType(keyType))
            {
                cppKeyType = convertName_baseTypes[keyType];
            }
            else
            {
                std::regex hasDot("(\\w+)\\.(\\w+)");
                std::smatch matches_dot;
                std::string nestedType;
                bool has_Dot = false;
                if(std::regex_match(keyType, matches_dot, hasDot))
                {
                    has_Dot = true;
                    keyType = matches_dot[1].str();
                    nestedType = matches_dot[2].str();
                }
                
                std::string siblingLocation = parentDir.compare("") != 0 ? parentDir + '_' + keyType : keyType;
                std::string childLocation = dir.compare("") != 0 ? dir + '_' + keyType : keyType;
                
                //import는 절대위치만 확인, enum 이나 struct 면 스코프를 확인 후 변환
                if(convertName_importedTypes.count(keyType) != 0)
                {
                    cppKeyType = convertName_importedTypes[keyType];
                    if(has_Dot)
                        cppKeyType += "::" + nestedType;
                    
					existing_importedTypes.insert(cppKeyType);

                    if(MY_DEBUG == 3)
                        std::cout << "line: " << linenum << " cppType: " << cppKeyType << '\n';
                }
                else if(convertName_enumTypes.count(childLocation) != 0)
                {
                    cppKeyType = convertName_enumTypes[childLocation];
                    if(has_Dot)
                        cppKeyType += "::" + nestedType;
                }
                else if(convertName_enumTypes.count(siblingLocation) != 0)
                {
                    cppKeyType = convertName_enumTypes[siblingLocation];
                    if(has_Dot)
                        cppKeyType += "::" + nestedType;
                }
                else if(existing_structTypes.count(childLocation) != 0)
                {
                    cppKeyType = childLocation;
                    if(has_Dot)
                        cppKeyType += '_' + nestedType;
                }
                else if(existing_structTypes.count(siblingLocation) != 0)
                {
                    cppKeyType = siblingLocation;
                    if(has_Dot)
                        cppKeyType += '_' + nestedType;
                }
                else
                {
                    std::cout << "ERROR: Unknown type '" << keyType << "' at line " << linenum << ".\n";
                }
            }
            
            //리스트 타입이면 listTypeMembers, 아니면 members 에 멤버 변수 추가
            if(std::regex_match(line, elem_list))
                listTypeMembers[dir].push_back(make_pair(matches[1].str(), cppKeyType));
            else
                members[dir].push_back(make_pair(matches[1].str(), cppKeyType));
            
            //만일 key 타입이면 keyName에 추가
            if(std::regex_match(line, elem_key))
            {
                keyTypes[dir] = cppKeyType;
                keyNames[dir] = matches[1].str();
            }
            
            
            if(MY_DEBUG == 2) std::cout << "elem" << '\n';
            if(MY_DEBUG == -1)
            {
                std::cout << members[dir].begin()->first << '\n';
                std::cout << members[dir].begin()->second << '\n';
            }
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
    
    std::cout << "ERROR: Unexpected end of file at line " << linenum << ".\n";
    return;
}

//1회차: 파일을 읽으며 구조체와 enum의 hierarchy 파악
void readStruct(std::ifstream& input, std::deque<std::string>& parents, std::string name, int& linenum)
{
    std::string parentDir = getKey(parents);
    std::string dir = getKey(parents, name);
    
    parents.push_back(name);
    
    //readStruct 의 목적1 - struct의 위치를 알아내고 existing_structTypes 에 저장
    existing_structTypes.insert(dir);
    
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
        else if(std::regex_match(line, matches, elem_key)   ||
                std::regex_match(line, matches, elem)       ||
                std::regex_match(line, matches, elem_list)  ||
                std::regex_match(line, matches, serverOnly))
        {
            continue;
        }
        else if(std::regex_match(line, matches, enum_usual))
        {
            readEnum(input, parents, matches[1].str(), linenum);
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
    
    std::cout << "ERROR: Unexpected end of file at line " << linenum << ".\n";
    return;
}

//헤더 파일 작성
void writeHeader()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ofstream h("gb" + rootName + "StaticDataManager.h");
	//std::ostream& h = std::cout;

	//주석 정보

	h << "//\n";
	h << "//  gb" + rootName + "StaticDataManager.h\n";
	h << "//  gbGame\n";
	h << "//\n";
	h << "//  Created by StaticDataManagerGenerator on " << std::put_time(&tm, "%Y. %m. %d") << "\n";
	h << "//\n\n";

	h << "#ifndef gb" + rootName + "StaticDataManager_h\n";
	h << "#define gb" + rootName + "StaticDataManager_h\n\n";

	h << "#include \"gbStaticDataManager.h\"\n";
	h << "#include \"capnp/gamedata/" + low(rootName) + ".capnp.h\n\n";

	h << "#define " + rootName + "StaticDataManagerInstance gb::gamedata::" + rootName + "::getInstance()\n\n";

	//import들 전방 선언
    h << WARNING + "\n";
	for (const auto& className : existing_importedTypes)
	{
		h << "class " + className + ";\n";
	}
	h << "\n";

	h << "NS_GB_GAMEDATA_BEGIN\n\n";

	//struct들 전방 선언
	for(const auto& className : existing_structTypes)
	{
		h << "class " + className + ";\n";
	}
	h << "\n";

	for (const auto& className : existing_structTypes)
	{
		if(className.compare(rootName) != 0)
			h << "class " + className + "\n";
		else
			h << "class " + className + " : public StaticDataManager<" + rootName + ">\n";
		h << "{\n";

		// public -> constructor 생성
		h << "public:\n";
		if (className.compare(rootName) != 0)
		{
			h << TAB + className + "(" + getReader(className) + " capnpReader);\n";
		}
		else
		{
			h << TAB + rootName + "();\n";
			h << TAB + "~" + rootName + "();\n";
		}

		// getter 생성
		for (const auto& memberVar : members[className])
		{
			if (!isCppStructType(memberVar.second))
			{
				// int64_t getStartTime() const { return _startTime; }
				h << TAB + memberVar.second + " get" + up(memberVar.first) + "() const { return _" + memberVar.first + "; }\n";
			}
			else
			{
				// const Row* getRowForOrder(const int16_t order) const;
				h << TAB + "const " + memberVar.second + "* get" + up(memberVar.first) +
					"() const { return _" + memberVar.first + ".get(); }\n";
			}
		}
		h << "\n";
		for (const auto& memberVar : listTypeMembers[className])
		{
			auto name = memberVar.first;
			auto type = memberVar.second;
			if (!hasKey(memberVar.second))
			{
				// const std::vector<MysteryMazeFollowerMapInfo>& getFollowerMapInfos() const { return _followerMapInfos; }
				h << TAB + "const std::vector<" + type + ">& get" + up(name) + "() const { return _" +
					name + "; }\n";
			}
			else
			{
				//
				std::string keyType = keyTypes[type];
				h << TAB + "const std::map<" + keyType + ", " + type + ">& getAll" + up(name) +
					"() const { return _" + name + "; }\n";
				if (!isCppBaseType(type))
				{
					//const MysteryMazeSeason* getMysteryMazeSeason(const int16_t id) const;
					//{
					//	auto it = _seasons.find(id);
					//	return it != _seasons.end() ? &it->second : nullptr;
					//}
					h << TAB + "const " + type + "* get" + up(name) + "(const " + keyType + " id) const\n" +
						TAB + "{\n" + 
						TAB + TAB + "auto it = _" + name + ".find(id);\n" +
						TAB + TAB + "return it != _" + name + ".end() ? & it->second : nullptr;\n" +
						TAB + "}\n";
				}
				else
				{
					h << TAB + type + " get" + up(name) + "(const " + keyType + " id) const\n" +
						TAB + "{\n" +
						TAB + TAB + "auto it = _" + name + ".find(id);\n" +
						TAB + TAB + "return it != _" + name + ".end() ? & it->second : nullptr;\n" +
						TAB + "}\n";
				}
			}
		}

		// protected -> load/unload 함수
		if (className.compare(rootName) == 0)
		{
			h << "protected:\n";
			h << TAB + "void loadStaticData() override;\n";
			h << TAB + "void unloadStaticData() override;\n\n";
		} 

		h << "private:\n";
		// 비 list 타입 멤버 변수
		for (const auto& memberVar : members[className])
		{
            if (isImportedType(memberVar.second))
            {
                h << WARNING + " ##" << memberVar.second << "##\n";
            }

			if(!isCppStructType(memberVar.second))
				h << TAB + memberVar.second + " _" + memberVar.first + ";\n";
			else
				h << TAB + "std::unique_ptr<" + memberVar.second + "> _" + memberVar.first + ";\n";
		}
		// list 타입 멤버 변수
		for (const auto& memberVar : listTypeMembers[className])
		{
			if (isImportedType(memberVar.second))
			{
				h << WARNING + " ##" << memberVar.second << "##\n";
			}

			if (!hasKey(memberVar.second))
				h << TAB + "std::vector<" + memberVar.second + "> _" + memberVar.first + ";\n";
			else
			{
				std::string keyType = keyTypes[memberVar.second];
				h << TAB + "std::map<" + keyType + ", " + memberVar.second + "> _" + memberVar.first + ";\n";
			}
		}
		h << "};\n\n";
	}

	h << "NS_GB_GAMEDATA_END\n\n";

	h << "#endif\n";
}

void writeCpp()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ofstream h("gb" + rootName + "StaticDataManager.cpp");
	//std::ostream& h = std::cout;

	//주석 정보

	h << "//\n";
	h << "//  gb" + rootName + "StaticDataManager.cpp\n";
	h << "//  gbGame\n";
	h << "//\n";
	h << "//  Created by StaticDataManagerGenerator on " << std::put_time(&tm, "%Y. %m. %d") << "\n";
	h << "//\n\n";

	h << "#include \"gb" + rootName + "StaticDataManager.h\"\n";
    h << "#include \"gbCapnpHelper.h\"\n\n";

	h << "NS_GB_GAMEDATA_BEGIN\n\n";

    h << rootName + "::" + rootName + "()\n";
    h << "{\n";
    h << TAB + "loadStaticData();\n";
    h << "}\n\n";
    
    h << rootName + "::~" + rootName + "()\n";
    h << "{\n";
    h << TAB + "unloadStaticData();\n";
    h << "}\n\n";
    
    h << "void " + rootName + "::loadStaticData()\n";
    h << "{\n";
    h << TAB + "auto capnpReader = CapnpHelper::FileReader<gb::capnp::gamedata::" + rootName +
        ">(\"" + low(rootName) + ".dxc\").getRoot();\n";
    for (const auto& memberVar : members[rootName])
    {
        auto name = memberVar.first;
        auto type = memberVar.second;
        if (isImportedType(type))
        {
            h << WARNING + " ##" << memberVar.second << "##\n";
        }
        
        if(!isCppStructType(type))
            h << TAB + " _" + name + " = capnpReader.get" + up(name) + "();\n";
        else
        {
            // , _followerCardConditions(std::make_unique<CardConditionHelper>(capnpReader.getFollowerCardConditions()))
            h << TAB + " _" + name + " = std::make_unique<" + type +
            ">(capnpReader.get" + up(name) + "());\n";
        }
    }
    for (const auto& memberVar : listTypeMembers[rootName])
    {
        auto name = memberVar.first;
        auto type = memberVar.second;
        if (isImportedType(type))
        {
            h << WARNING + " ##" << memberVar.second << "##\n";
        }
        
        if (!hasKey(memberVar.second))
        {
            h << TAB + "for (auto capnp : capnpReader.get" + up(name) + "())\n";
            h << TAB + "{\n";
            h << TAB + TAB + "_" + name + ".emplace_back(capnp);\n";
            h << TAB + "}\n";
        }
        else
        {
            h << TAB + "for (auto capnp : capnpReader.get" + up(name) + "())\n";
            h << TAB + "{\n";
            h << TAB + TAB + "_" + name + ".emplace(capnp.get" + up(keyNames[type]) +
            "(), " + type + "(capnp));\n";
            h << TAB + "}\n";
        }
    }
    h << "}\n\n";
    
    h << "void " + rootName + "::unloadStaticData()\n";
    h << "{\n";
    for (const auto& memberVar : listTypeMembers[rootName])
    {
        auto name = memberVar.first;
        auto type = memberVar.second;
        h << TAB + "_" + name + ".clear();\n";
    }
    h << "}\n\n";
    
    for (const auto& className : existing_structTypes)
    {
        if(className == rootName)
            continue;
        
        std::string starter = ":";
        h << className + "::" + className + "(" + getReader(className) + " capnpReader)\n";
        for (const auto& memberVar : members[className])
        {
            auto name = memberVar.first;
            auto type = memberVar.second;
            
            if (isImportedType(type))
            {
                h << WARNING + " ##" << memberVar.second << "##\n";
            }
            
            if(!isCppStructType(type))
                h << starter + " _" + name + "(capnpReader.get" + up(name) + "())\n";
            else
            {
                // , _followerCardConditions(std::make_unique<CardConditionHelper>(capnpReader.getFollowerCardConditions()))
                h << starter + " _" + name + "(std::make_unique<" + type +
                ">(capnpReader.get" + up(name) + "()))\n";
            }
            starter = ",";
        }
        h << "{\n";
        
        for (const auto& memberVar : listTypeMembers[className])
        {
            auto name = memberVar.first;
            auto type = memberVar.second;
            
            if (isImportedType(type))
            {
                h << WARNING + " ##" << memberVar.second << "##\n";
            }
            
            if (!hasKey(memberVar.second))
            {
                h << TAB + "for (auto capnp : capnpReader.get" + up(name) + "())\n";
                h << TAB + "{\n";
                h << TAB + TAB + "_" + name + ".emplace_back(capnp);\n";
                h << TAB + "}\n";
            }
            else
            {
                h << TAB + "for (auto capnp : capnpReader.get" + up(name) + "())\n";
                h << TAB + "{\n";
                h << TAB + TAB + "_" + name + ".emplace(capnp.get" + up(keyNames[type]) +
                    ", " + type + "(capnp));\n";
                h << TAB + "}\n";
            }
        }
        h << "}\n\n";
    }


    h << "NS_GB_GAMEDATA_END\n";
}

int main(int argc, const char * argv[])
{
    insertBaseTypes();
    
    std::string fileDir = "Test.capnp";
    if(argc < 1)
    {
        
        std::cout << "ERROR: No file name given. Using \"Test.capnp\".\n";
        //return 0;
    }
    else
    {
        fileDir = argv[1];
    }
    std::ifstream input(fileDir);
    if(!input.is_open())
    {
        std::cout << "ERROR: file not found\n";
    }
    
    if(MY_DEBUG)
    {
        char path[1024] = "unavailable";
        getcwd(path, sizeof(path));
        std::cout << "Current working dir is " << path << '\n';
        std::cout << "File name: " << fileDir << std::endl;
    }
    
    std::string line;
    int linenum = 0;
    
    /* import 처리 + 1회차: hierarchy 파악 */
    while(getline(input, line))
    {
        linenum++;
        std::smatch matches;
        if(linenum <= LINES_TO_IGNORE               ||
           std::regex_match(line, matches, comment) ||
           std::regex_match(line, matches, whiteSpace)) //무시 대상
        {
            continue;
        }
        else if(std::regex_match(line, matches, import) ||
                std::regex_match(line, matches, import2)) //import
        {
            std::string myName = matches[1].str();
            std::string toImport;
            if(std::regex_match(line, import))
                toImport = matches[2].str();
            else
                toImport = matches[1].str();
            
            std::regex hasDot("([\"./\\w]+)\\.(\\w+)");
            std::smatch matches_dot;
            std::string nestedType;
            bool has_Dot = false;
            if(std::regex_match(toImport, matches_dot, hasDot))
            {
                has_Dot = true;
                nestedType = matches_dot[2].str();
				toImport = matches_dot[1].str();
                if(std::regex_match(line, import2))
                    myName = nestedType;
            }
            
            std::regex clean("\"([./\\w]+)\\.capnp\"");
            std::smatch cleanResult;
            if(std::regex_match(toImport, cleanResult, clean))
            {
                toImport = cleanResult[1].str();
            }
            else
            {
                std::cout << "ERROR: Unknown pattern found, failed to parse line " << linenum << ".\n";
                std::cout << "The line at fault: \n" << line << "\n\n";
                return 0;
            }
            
            std::replace(toImport.begin(), toImport.end(), '/', ' ');
            std::vector<std::string> array;
            std::stringstream ss(toImport);
            std::string token;
            while(ss >> token)
                array.push_back(token);
            if(array.size() > 0)
                array.pop_back();
            
            if(has_Dot)
                array.push_back(nestedType);
            
            std::string cppType = "gb::capnp";
            int p = 0;
            if(array[0].compare("..") != 0)
            {
                cppType += "::gamedata";
            }
            else
                p++;
            
            for(; p < array.size(); p++)
            {
                cppType += "::" + array[p];
            }
            
            convertName_importedTypes[myName] = cppType;
            
            if(MY_DEBUG == 3)
                std::cout << "myName: " << myName << " cppType: " << cppType << '\n';
        }
        else if(std::regex_match(line, matches, struct_root) ||
                std::regex_match(line, matches, struct_usual)) //struct_root
        {
            //루트 struct면 rootName 에 추가
            if(std::regex_match(line, struct_root))
               rootName = matches[1].str();
            
            std::deque<std::string> parents = std::deque<std::string>();
            readStruct(input, parents, matches[1].str(), linenum);
        }
        else if(std::regex_match(line, matches, enum_usual))
        {
            std::deque<std::string> parents = std::deque<std::string>();
            readEnum(input, parents, matches[1].str(), linenum);
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
    
    /* 2회차 - 멤버 변수 파악 */
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
            std::deque<std::string> parents = std::deque<std::string>();
            readElem(input, parents, matches[1].str(), linenum);
        }
        else if(std::regex_match(line, matches, enum_usual))
        {
            std::deque<std::string> parents = std::deque<std::string>();
            readEnum(input, parents, matches[1].str(), linenum);
        }
        else
        {
            std::cout << "ERROR: Unknown pattern found, failed to parse line " << linenum << ".\n";
            std::cout << "The line at fault: \n" << line << "\n\n";
            return 0;
        }
    }

	writeHeader();

	writeCpp();

    return 0;
}

std::string low(std::string orig)
{
	std::string ret = orig;
	ret[0] = tolower(ret[0]);
	return ret;
}

std::string up(std::string orig)
{
	std::string ret = orig;
	ret[0] = toupper(ret[0]);
	return ret;
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}
