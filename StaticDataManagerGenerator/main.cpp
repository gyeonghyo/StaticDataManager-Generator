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

#define MY_DEBUG 0
#define LINES_TO_IGNORE 11
#define TAB (std::string)"    "
#define WARNING (std::string)"CHECK IMPORTED TYPE"

//using namespace std;

std::regex complexComment("(.*)#\\s*(.*)");
std::regex comment("\\s*#\\s*(.*)");                                                          // # 시즌 종료 후 정산하는데 쓸 여유 시간
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
std::map<std::string, std::map<std::string, std::vector<std::string>>> memberComments;   //capnpName -> comment
std::map<std::string, std::vector<std::string>> structComments;  //cppName -> comment


/* elem - cppType*/
std::set<std::string> baseTypes;

std::map<std::string, std::string> SpecialImport_type;  //저장 type
std::map<std::string, std::string> SpecialImport_returnType;
std::map<std::string, std::string> SpecialImport_pureType;
std::map<std::string, std::string> SpecialImport_getter;
std::map<std::string, std::set<std::string>> SpecialImport_header; //cpp용 헤더들
std::map<std::string, std::set<std::string>> SpecialImport_forward; //전방 선언

// struct 별 dependency 추적
std::map<std::string, std::set<std::string>> to;
std::map<std::string, std::set<std::string>> from;

std::string rootName;

//options
bool option_pointer = true;
enum class pointerOption{unique, shared};
pointerOption myPO = pointerOption::shared;

bool option_macro = true;
bool option_autoImport = true;
bool option_eraseUnderScore = true;

//util
std::string low(std::string orig);
std::string up(std::string orig);
std::string replaceAll(std::string str, const std::string& from, const std::string& to);
std::string singular(std::string name);
std::deque<std::string> topologicalSort();

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

bool isSpecialImportedType(std::string cppType)
{
    return SpecialImport_type.count(cppType) != 0;
}

//key 가 있는가?
bool hasKey(std::string cppTypeName)
{
	return keyTypes.count(cppTypeName) != 0;
}

std::string er(std::string s)
{
    if(isCppBaseType(s))
        return s;
    if(!option_eraseUnderScore)
        return s;
    std::string str = s;
    str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
    return str;
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
    convertName_baseTypes["UInt64"] = "uint64_t";
    convertName_baseTypes["UInt32"] = "uint32_t";
    convertName_baseTypes["UInt16"] = "uint16_t";
    convertName_baseTypes["UInt8"] = "uint8_t";
	for (const auto& pa : convertName_baseTypes)
	{
		baseTypes.insert(pa.second);
	}
}

void insertSpecialImports()
{
    if(!option_autoImport)
        return;
    
    SpecialImport_type["gb::capnp::shared::ItemData"] = "std::unique_ptr<item::Item>";
    SpecialImport_returnType["gb::capnp::shared::ItemData"] = "item::Item*";
    SpecialImport_pureType["gb::capnp::shared::ItemData"] = "item::Item";
    SpecialImport_getter["gb::capnp::shared::ItemData"] = "item::ItemCapnpHelper::makeItem";
    SpecialImport_header["gb::capnp::shared::ItemData"].insert("gbItemCapnpHelper.h");
    SpecialImport_forward["gb::capnp::shared::ItemData"].insert(
    "NS_GB_ITEM_BEGIN\n"
    "class Item;\n"
    "NS_GB_ITEM_END\n");

    SpecialImport_type["gb::capnp::gamedata::Price"] = "std::unique_ptr<shop::Price>";
    SpecialImport_returnType["gb::capnp::gamedata::Price"] = "shop::Price*";
    SpecialImport_pureType["gb::capnp::gamedata::Price"] = "shop::Price";
    SpecialImport_getter["gb::capnp::gamedata::Price"] = "shop::PriceCapnpHelper::makePrice";
    SpecialImport_header["gb::capnp::gamedata::Price"].insert("gbPriceCapnpHelper.h");
    SpecialImport_forward["gb::capnp::gamedata::Price"].insert(
    "NS_GB_GAMEDATA_SHOP_BEGIN\n"
    "class Price;\n"
    "NS_GB_GAMEDATA_SHOP_END\n");
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

bool extractComment(std::string& line, std::string& com)
{
    std::smatch matches;
    std::string line2 = line;
    if(std::regex_match(line2, comment))
       return false;
    
    if(std::regex_match(line2, matches, complexComment))
    {
        com = matches[2].str();
        line = matches[1].str();
        return true;
    }
    return false;
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
        std::string compComment = "";
        extractComment(line, compComment);
        
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
    std::string prevMember = "";
    while(getline(input, line))
    {
        std::string compComment = "";
        bool hasCompComment = extractComment(line, compComment);
        
        linenum++;
        std::smatch matches;
        
        if(std::regex_match(line, matches, whiteSpace) ||
           std::regex_match(line, matches, serverOnly))
        {
            continue;
        }
        if(std::regex_match(line, matches, comment))
        {
            std::string com = matches[1].str();
            if(prevMember.compare("") == 0)
            {
                structComments[dir].push_back(com);
            }
            else
            {
                memberComments[dir][prevMember].push_back(com);
            }
        }
        else if(std::regex_match(line, matches, struct_usual))
        {
            readElem(input, parents, matches[1].str(), linenum);
            if(MY_DEBUG == 2) std::cout << "struct_usual" << '\n';
            
            if(hasCompComment)
            {
                std::string com = compComment;
                std::string childName = dir.compare("") == 0 ? matches[1].str() : dir + "_" + matches[1].str();
                structComments[childName].push_back(com);
                hasCompComment = false;
            }
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
            prevMember = matches[1].str();
            
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
                    nestedType = matches_dot[2].str();
                    keyType = matches_dot[1].str();
                }
                
                /*
                std::string siblingLocation = parentDir.compare("") != 0 ? parentDir + '_' + keyType : keyType;
                std::string childLocation = dir.compare("") != 0 ? dir + '_' + keyType : keyType;
                */
                 
                std::deque<std::string> tmpParents;
                std::deque<std::string> scopes;
                for (auto st : parents)
                {
                    std::string k = getKey(tmpParents);
                    scopes.push_front(k.compare("") != 0 ? k + '_' + keyType : keyType);
                    tmpParents.push_back(st);
                }
                std::string k = getKey(tmpParents);
                scopes.push_front(k.compare("") != 0 ? k + '_' + keyType : keyType);
                
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
                else
                {
                    //자신의 자식이나, 자신의 조상의 자식에 대해서만 scope를 제한 후 Type를 검색
                    bool found = false;
                    for(const auto & scopeName : scopes)
                    {
                        //std::cout << scopeName << '\n';
                        if(convertName_enumTypes.count(scopeName) != 0)
                        {
                            cppKeyType = convertName_enumTypes[scopeName];
                            if(has_Dot)
                                cppKeyType += "::" + nestedType;
                            found = true;
                            break;
                        }
                        else if(existing_structTypes.count(scopeName) != 0)
                        {
                            cppKeyType = scopeName;
                            if(has_Dot)
                                cppKeyType += '_' + nestedType;
                            found = true;
                            
                            //dependency check
                            to[dir].insert(cppKeyType);
                            from[cppKeyType].insert(dir);
                            
                            break;
                        }
                    }
                    if(!found)
                    {
                        std::cout << "ERROR: Unknown type '" << keyType << "' at line " << linenum << ".\n";
                    }
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
        
        if(hasCompComment)
        {
            std::string com = compComment;
            if(prevMember.compare("") == 0)
            {
                structComments[dir].push_back(com);
            }
            else
            {
                memberComments[dir][prevMember].push_back(com);
            }
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
        std::string compComment = "";
        extractComment(line, compComment);
        
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
bool writeHeader(std::deque<std::string>& structs)
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

    std::string fileDir = "gb" + rootName + "StaticDataManager.h";
    std::string fileDir2 = "gb" + rootName + "StaticDataManager.cpp";
    if (FILE *file = fopen(fileDir.c_str(), "r"))
    {
        std::string inp;
        fclose(file);
        std::cout << "WARNING: The file " + fileDir + " already exists.\n";
        std::cout << "Do you wish to overwrite? (y or n)" << std::endl << std::flush;
        std::getline(std::cin, inp);
        if(!(inp.compare("Y") == 0 || inp.compare("y") == 0))
        {
            std::cout << "aborting process...\n";
            return false;
        }
    }
    else if(FILE *file = fopen(fileDir2.c_str(), "r"))
    {
        std::string inp;
        fclose(file);
        std::cout << "WARNING: The file " + fileDir2 + " already exists.\n";
        std::cout << "Do you wish to overwrite? (y or n)" << std::endl << std::flush;
        std::getline(std::cin, inp);
        if(!(inp.compare("Y") == 0 || inp.compare("y") == 0))
        {
            std::cout << "aborting process...\n";
            return false;
        }
    }
    
	std::ofstream h(fileDir);
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
    h << "#include \"capnp/gamedata/" + low(rootName) + ".capnp.h\"\n";
    if(option_macro)
    {
        h << "#include \"gbMacros.h\"\n";
    }
    h << "\n";

	h << "#define " + rootName + "StaticDataManagerInstance gb::gamedata::" + rootName +
        "StaticDataManager::getInstance()\n\n";
    
    for(auto& im : SpecialImport_forward)
    {
        if(existing_importedTypes.count(im.first) == 0)
            continue;
        for(auto forw : im.second)
            h << forw << '\n';
    }
    
	h << "NS_GB_GAMEDATA_BEGIN\n\n";

	for (const auto& className : structs)
	{
        if(structComments.count(className) != 0)
        {
            for(const auto& line : structComments[className])
                h << "// " +  line + '\n';
        }
        
		if(className.compare(rootName) != 0)
			h << "class " + er(className) + "\n";
		else
			h << "class " + er(className) + "StaticDataManager \n: public StaticDataManager<" + rootName +
                "StaticDataManager>\n";
		h << "{\n";

		// public -> constructor 생성
		h << "public:\n";
		if (className.compare(rootName) != 0)
		{
			h << TAB + er(className) + "(const " + getReader(className) + "& capnpReader);\n";
		}
		else
		{
			h << TAB + rootName + "StaticDataManager();\n";
			h << TAB + "~" + rootName + "StaticDataManager();\n";
		}

		// getter 생성
		for (const auto& memberVar : members[className])
		{
            auto name = memberVar.first;
            auto type = memberVar.second;
            
            if (memberComments.count(className) != 0 && memberComments[className].count(name) != 0)
            {
                for(const auto& line : memberComments[className][name])
                    h << TAB + "// " + line + '\n';
            }
            
            if (isImportedType(memberVar.second))
            {
                h << WARNING + " ##" << memberVar.second << "##\n";
            }
            
            if (isCppBaseType(type) && option_macro)
            {
                if(type.compare("std::string") == 0)
                    h << TAB + "GB_SYNTHESIZE_READONLY_PASS_BY_REF(" + er(type) + ", _" + name + ", " + up(name) + ");\n";
                else
                    h << TAB + "GB_SYNTHESIZE_READONLY(" + er(type) + ", _" + name + ", " + up(name) + ");\n";
                continue;
            }
            
            std::string retType = type;
            if(isSpecialImportedType(type))
            {
                retType = SpecialImport_returnType[type];
                h << TAB + retType + " get" + up(memberVar.first) + "() const { return _" + memberVar.first + ".get(); }\n";
            }
			else if (!isCppStructType(type))
			{
				// int64_t getStartTime() const { return _startTime; }
				h << TAB + retType + " get" + up(memberVar.first) + "() const { return _" + memberVar.first + "; }\n";
			}
			else
			{
				// const Row* getRowForOrder(const int16_t order) const;
				h << TAB + "const " + er(retType) + "* get" + up(memberVar.first) +
					"() const { return _" + memberVar.first + ".get(); }\n";
			}
		}
		for (const auto& memberVar : listTypeMembers[className])
		{
			auto name = memberVar.first;
			auto type = memberVar.second;
            
            if (memberComments.count(className) != 0 && memberComments[className].count(name) != 0)
            {
                for(const auto& line : memberComments[className][name])
                    h << TAB + "// " + line + '\n';
            }
            
            if (isImportedType(type))
            {
                h << WARNING + " ##" << memberVar.second << "##\n";
            }
            std::string pointerType = type;
			if (!hasKey(memberVar.second))
			{
                // const std::vector<std::unique_ptr<MysteryMazeFollowerMapInfo>>& getFollowerMapInfos() const { return _followerMapInfos; }
                
                if(isCppStructType(type))
                {
                    if(option_pointer)
                    {
                        switch (myPO)
                        {
                        case pointerOption::unique:
                            pointerType = "std::unique_ptr<" + er(type) + ">";
                            break;
                        case pointerOption::shared:
                            pointerType = "std::shared_ptr<" + er(type) + ">";
                            break;
                        }
                    }
                    else
                    {
                        pointerType = er(type);
                    }
                }
                else if(isSpecialImportedType(type))
                {
                    pointerType = SpecialImport_type[type];
                }
                
                if(option_macro)
                {
                    h << TAB + "GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::vector<" + pointerType + ">, _" + name + ", " + up(name) + ");\n";
                    continue;
                }
				h << TAB + "const std::vector<" + pointerType + ">& get" + up(name) + "() const { return _" +
					name + "; }\n";
			}
			else
			{
				std::string keyType = keyTypes[type];
                std::string retType = type;
                if(isCppStructType(type))
                {
                    if(option_pointer)
                    {
                        switch (myPO)
                        {
                            case pointerOption::unique:
                                pointerType = "std::unique_ptr<" + er(type) + ">";
                                break;
                            case pointerOption::shared:
                                pointerType = "std::shared_ptr<" + er(type) + ">";
                                break;
                        }
                    }
                    else
                    {
                        pointerType = er(type);
                    }
                    retType = er(type);
                }
                else if(isSpecialImportedType(type))
                {
                    pointerType = SpecialImport_type[type];
                    retType = SpecialImport_pureType[type];
                }
                
                //TODO: retType 가 basic 이면?
                
				h << TAB + "const std::map<" + keyType + ", " + pointerType + ">& getAll" + up(name) +
					"() const { return _" + name + "; }\n";
                h << TAB + "const " + retType + "* get" + up(singular(name)) + "(const " +
                    keyType + " id) const;\n";
                
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
            auto type = memberVar.second;
            auto saveType = type;
            if (isCppBaseType(type) && option_macro)
                continue;
            if (isImportedType(type))
            {
                h << WARNING + " ##" << type << "##\n";
                if(isSpecialImportedType(type))
                {
                    saveType = SpecialImport_type[type];
                }
            }

			if(!isCppStructType(type))
				h << TAB + saveType + " _" + memberVar.first + ";\n";
			else
				h << TAB + "std::unique_ptr<" + er(type) + "> _" + memberVar.first + ";\n";
		}
		// list 타입 멤버 변수
		for (const auto& memberVar : listTypeMembers[className])
		{
            auto name = memberVar.first;
            auto type = memberVar.second;
            std::string pointerType = type;
            if(isCppStructType(type))
            {
                if(option_pointer)
                {
                    switch (myPO)
                    {
                        case pointerOption::unique:
                            pointerType = "std::unique_ptr<" + er(type) + ">";
                            break;
                        case pointerOption::shared:
                            pointerType = "std::shared_ptr<" + er(type) + ">";
                            break;
                    }
                }
                else
                {
                    pointerType = er(type);
                }
            }
            
            if (!hasKey(memberVar.second) && option_macro)
                continue;
            
			if (isImportedType(memberVar.second))
			{
				h << WARNING + " ##" << memberVar.second << "##\n";
                if(isSpecialImportedType(type))
                    pointerType = SpecialImport_type[type];
			}

			if (!hasKey(memberVar.second))
            {
				h << TAB + "std::vector<" + pointerType + "> _" + memberVar.first + ";\n";
            }
			else
			{
				std::string keyType = keyTypes[memberVar.second];
				h << TAB + "std::map<" + keyType + ", " + pointerType + "> _" + memberVar.first + ";\n";
			}
		}
		h << "};\n\n";
	}

	h << "NS_GB_GAMEDATA_END\n\n";

	h << "#endif\n";
    return true;
}

void writeCpp(std::deque<std::string>& structs)
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
    h << "#include \"gbCapnpHelper.h\"\n";
    for(auto im : SpecialImport_header)
    {
        if(existing_importedTypes.count(im.first) == 0)
            continue;
        for(auto& head : im.second)
        {
            h << "#include \"" + head + "\"\n";
        }
    }
    h << "\n";
    
	h << "NS_GB_GAMEDATA_BEGIN\n\n";

    h << rootName + "StaticDataManager::" + rootName + "StaticDataManager()\n";
    h << "{\n";
    h << TAB + "loadStaticData();\n";
    h << "}\n\n";
    
    h << rootName + "StaticDataManager::~" + rootName + "StaticDataManager()\n";
    h << "{\n";
    h << TAB + "unloadStaticData();\n";
    h << "}\n\n";
    
    h << "void " + rootName + "StaticDataManager::loadStaticData()\n";
    h << "{\n";
    h << TAB + "auto fileReader = CapnpHelper::FileReader<gb::capnp::gamedata::" + rootName +
        ">(\"" + low(rootName) + ".dxc\");\n";
    h << TAB + "auto capnpReader = fileReader.getRoot();\n";
    for (const auto& memberVar : members[rootName])
    {
        auto name = memberVar.first;
        auto type = memberVar.second;
        if (isImportedType(type))
        {
            h << WARNING + " ##" << memberVar.second << "##\n";
        }
        
        if(isSpecialImportedType(type))
        {
            h << TAB + " _" + name + " = " + SpecialImport_getter[type] +
                "(capnpReader.get" + up(name) + "());\n";
        }
        else if(!isCppStructType(type))
            h << TAB + " _" + name + " = capnpReader.get" + up(name) + "();\n";
        else
        {
            // , _followerCardConditions(std::make_unique<CardConditionHelper>(capnpReader.getFollowerCardConditions()))
            h << TAB + " _" + name + " = std::make_unique<" + er(type) +
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
        std::string pointerType = type;
        if(isCppStructType(type))
        {
            if(option_pointer)
            {
                switch (myPO)
                {
                    case pointerOption::unique:
                        pointerType = "std::make_unique<" + er(type) + ">";
                        break;
                    case pointerOption::shared:
                        pointerType = "std::make_shared<" + er(type) + ">";
                        break;
                }
            }
            else
            {
                pointerType = er(type);
            }
        }
        if(isSpecialImportedType(type))
        {
            pointerType = SpecialImport_getter[type];
        }
        
        if (!hasKey(memberVar.second))
        {
            h << TAB + "for (auto capnp : capnpReader.get" + up(name) + "())\n";
            h << TAB + "{\n";
            h << TAB + TAB + "_" + name + ".emplace_back(" + pointerType + "(capnp));\n";
            h << TAB + "}\n";
        }
        else
        {
            h << TAB + "for (auto capnp : capnpReader.get" + up(name) + "())\n";
            h << TAB + "{\n";
            h << TAB + TAB + "_" + name + ".emplace(capnp.get" + up(keyNames[type]) +
            "(), " + pointerType + "(capnp));\n";
            h << TAB + "}\n";
        }
    }
    h << "}\n\n";
    
    h << "void " + rootName + "StaticDataManager::unloadStaticData()\n";
    h << "{\n";
    for (const auto& memberVar : listTypeMembers[rootName])
    {
        auto name = memberVar.first;
        auto type = memberVar.second;
        h << TAB + "_" + name + ".clear();\n";
    }
    h << "}\n\n";
    
    for (const auto& className : structs)
    {
        if(className == rootName)
            continue;
        
        std::string starter = ":";
        h << er(className) + "::" + er(className) + "(const " + getReader(className) + "& capnpReader)\n";
        for (const auto& memberVar : members[className])
        {
            auto name = memberVar.first;
            auto type = memberVar.second;
            
            if (isImportedType(type))
            {
                h << WARNING + " ##" << memberVar.second << "##\n";
            }
            
            if(isSpecialImportedType(type))
            {
                auto getter = SpecialImport_getter[type];
                h << starter + " _" + name + "(" + getter + "(capnpReader.get" + up(name) + "()))\n";
            }
            else if(!isCppStructType(type))
                h << starter + " _" + name + "(capnpReader.get" + up(name) + "())\n";
            else
            {
                // , _followerCardConditions(std::make_unique<CardConditionHelper>(capnpReader.getFollowerCardConditions()))
                h << starter + " _" + name + "(std::make_unique<" + er(type) +
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
            std::string pointerType = "";
            if(isCppStructType(type))
            {
                if(option_pointer)
                {
                    switch (myPO)
                    {
                        case pointerOption::unique:
                            pointerType = "std::make_unique<" + er(type) + ">";
                            break;
                        case pointerOption::shared:
                            pointerType = "std::make_shared<" + er(type) + ">";
                            break;
                    }
                }
                else
                {
                    pointerType = er(type);
                }
            }
            if(isSpecialImportedType(type))
            {
                pointerType = SpecialImport_getter[type];
            }
            
            if (!hasKey(type))
            {
                h << TAB + "for (auto capnp : capnpReader.get" + up(name) + "())\n";
                h << TAB + "{\n";
                h << TAB + TAB + "_" + name + ".emplace_back(" + pointerType + "(capnp));\n";
                h << TAB + "}\n";
            }
            else
            {
                h << TAB + "for (auto capnp : capnpReader.get" + up(name) + "())\n";
                h << TAB + "{\n";
                h << TAB + TAB + "_" + name + ".emplace(capnp.get" + up(keyNames[type]) +
                    "(), " + pointerType + "(capnp));\n";
                h << TAB + "}\n";
            }
        }
        h << "}\n\n";
    }
    
    for(const auto& className : structs)
    {
        for(const auto& memberVar : listTypeMembers[className])
        {
            auto name = memberVar.first;
            auto type = memberVar.second;
            if(hasKey(type))
            {
                std::string keyType = keyTypes[type];
                std::string returnMethod = "it->second.get()";
                std::string classNameDisplay = er(className);
                if(classNameDisplay.compare(rootName) == 0)
                {
                    classNameDisplay += "StaticDataManager";
                }
                if(!option_pointer)
                {
                    returnMethod = "& it->second";
                }
                //const MysteryMazeSeason* getMysteryMazeSeason(const int16_t id) const;
                //{
                //    auto it = _seasons.find(id);
                //    return it != _seasons.end() ? it->second.get() : nullptr;
                //}
                if (isCppStructType(type))
                {
                    h << "const " + er(type) + "* " + classNameDisplay
                    + "::get" + up(singular(name)) + "(const " + keyType + " id) const\n" +
                    "{\n" +
                    TAB + "auto it = _" + name + ".find(id);\n" +
                    TAB + "return it != _" + name + ".end() ? " + returnMethod +
                        " : nullptr;\n" +
                    "}\n";
                }
                else
                {
                    h << er(type) + " " + classNameDisplay + "::get" + up(singular(name)) + "(const " + keyType + " id) const\n" +
                    "{\n" +
                    TAB + "auto it = _" + name + ".find(id);\n" +
                    TAB + "return it != _" + name + ".end() ? it->second : nullptr;\n" +
                    "}\n";
                }
            }
        }
    }

    h << "\nNS_GB_GAMEDATA_END\n";
}

int main(int argc, const char * argv[])
{
    std::cout << "starting conversion...\n";
    
    insertBaseTypes();
    insertSpecialImports();
    
    std::string fileDir = "Test.capnp";
    if(argc < 2)
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
    
    
    for(int arg = 2; arg < argc; arg++)
    {
        std::string str(argv[arg]);
        if(str.compare("-u") == 0)
        {
            myPO = pointerOption::unique;
        }
        else if(str.compare("-np") == 0)
        {
            option_pointer = false;
        }
        else if(str.compare("-nm") == 0)
        {
            option_macro = false;
        }
        else if(str.compare("-ni") == 0)
        {
            option_autoImport = false;
        }
        else if(str.compare("-us") == 0)
        {
            option_eraseUnderScore = false;
        }
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
        std::string compComment = "";
        extractComment(line, compComment);
        
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
        std::string compComment = "";
        extractComment(line, compComment);
        
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
    
    //root에서 도달할 수 없는 struct 제거/ dependency graph 체크 후 올바른 순서로 위상 정렬
    auto validStructs = topologicalSort();

    char path[1024] = "unavailable";
    getcwd(path, sizeof(path));
    
	if(!writeHeader(validStructs))
        return 0;

    std::cout << path << "/" + rootName + "StaticDataManager.h\n";
    
	writeCpp(validStructs);
    
    std::cout << path << "/" + rootName + "StaticDataManager.cpp\n";
    std::cout << "finished\n";
    
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

std::string singular(std::string name)
{
    if(name[name.length() - 1] == 's')
        return name.substr(0, name.length() - 1);
    else
        return name;
}

std::deque<std::string> topologicalSort()
{
    std::deque<std::string> ret;
    std::deque<std::string> toRemove;
    for(const auto& st : existing_structTypes)
    {
        if(to.count(st) == 0)
        {
            if(from[st].size() == 0 && st.compare(rootName) != 0)
                continue;
            toRemove.push_back(st);
        }
    }
    while(!toRemove.empty())
    {
        std::string rem = toRemove.front();
        toRemove.pop_front();
        if(from[rem].size() == 0 && rem.compare(rootName) != 0)
            continue;
        
        ret.push_back(rem);
        
        for(const auto& myFrom : from[rem])
        {
            to[myFrom].erase(rem);
            if(to[myFrom].size() == 0)
                toRemove.push_back(myFrom);
        }
    }
    return ret;
}
