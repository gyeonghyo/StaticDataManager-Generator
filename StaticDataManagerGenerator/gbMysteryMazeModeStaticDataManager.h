//
//  gbMysteryMazeModeStaticDataManager.h
//  gbGame
//
//  Created by StaticDataManagerGenerator on 2019. 04. 29
//

#ifndef gbMysteryMazeModeStaticDataManager_h
#define gbMysteryMazeModeStaticDataManager_h

#include "gbStaticDataManager.h"
#include "capnp/gamedata/mysteryMazeMode.capnp.h

#define MysteryMazeModeStaticDataManagerInstance gb::gamedata::MysteryMazeMode::getInstance()

PLEASE CHECK IMPORTED TYPE
class gb::capnp::gamedata::Action;
class gb::capnp::gamedata::CardCondition;
class gb::capnp::gamedata::QuestCondition;
class gb::capnp::gamedata::QuestGoal;
class gb::capnp::shared::ItemData;

NS_GB_GAMEDATA_BEGIN

class MysteryMazeMode;
class MysteryMazeMode_MazeTable;
class MysteryMazeMode_MazeTable_FollowerMapInfo;
class MysteryMazeMode_MazeTable_KeyOwnerMapInfo;
class MysteryMazeMode_MazeTable_Quest;
class MysteryMazeMode_Season;
class MysteryMazeMode_SeasonRewardTable;
class MysteryMazeMode_SeasonRewardTable_Row;

class MysteryMazeMode : public StaticDataManager<MysteryMazeMode>
{
public:
    MysteryMazeMode();
    ~MysteryMazeMode();
    int64_t getSeasonRewardCalcDuration() const { return _seasonRewardCalcDuration; }

    const std::map<int16_t, MysteryMazeMode_Season>& getAllSeasons() const { return _seasons; }
    const MysteryMazeMode_Season* getSeasons(const int16_t id) const
    {
        auto it = _seasons.find(id);
        return it != _seasons.end() ? & it->second : nullptr;
    }
    const std::map<int16_t, MysteryMazeMode_SeasonRewardTable>& getAllSeasonRewardTables() const { return _seasonRewardTables; }
    const MysteryMazeMode_SeasonRewardTable* getSeasonRewardTables(const int16_t id) const
    {
        auto it = _seasonRewardTables.find(id);
        return it != _seasonRewardTables.end() ? & it->second : nullptr;
    }
    const std::map<int16_t, MysteryMazeMode_MazeTable>& getAllMazeTables() const { return _mazeTables; }
    const MysteryMazeMode_MazeTable* getMazeTables(const int16_t id) const
    {
        auto it = _mazeTables.find(id);
        return it != _mazeTables.end() ? & it->second : nullptr;
    }
protected:
    void loadStaticData() override;
    void unloadStaticData() override;

private:
    int64_t _seasonRewardCalcDuration;
    std::map<int16_t, MysteryMazeMode_Season> _seasons;
    std::map<int16_t, MysteryMazeMode_SeasonRewardTable> _seasonRewardTables;
    std::map<int16_t, MysteryMazeMode_MazeTable> _mazeTables;
};

class MysteryMazeMode_MazeTable
{
public:
    MysteryMazeMode_MazeTable(gb::capnp::gamedata::MysteryMazeMode::MazeTable::Reader capnpReader);
    int16_t getId() const { return _id; }
    int32_t getPointJellyId() const { return _pointJellyId; }
    const MysteryMazeMode_MazeTable_KeyOwnerMapInfo* getKeyOwnerMapInfo() const { return _keyOwnerMapInfo.get(); }

    const std::map<int16_t, MysteryMazeMode_MazeTable_Quest>& getAllKeyOwnerQuests() const { return _keyOwnerQuests; }
    const MysteryMazeMode_MazeTable_Quest* getKeyOwnerQuests(const int16_t id) const
    {
        auto it = _keyOwnerQuests.find(id);
        return it != _keyOwnerQuests.end() ? & it->second : nullptr;
    }
    const std::vector<gb::capnp::gamedata::CardCondition>& getKeyOwnerCardConditions() const { return _keyOwnerCardConditions; }
    const std::vector<MysteryMazeMode_MazeTable_FollowerMapInfo>& getFollowerMapInfos() const { return _followerMapInfos; }
    const std::vector<gb::capnp::gamedata::CardCondition>& getFollowerCardConditions() const { return _followerCardConditions; }
private:
    int16_t _id;
    int32_t _pointJellyId;
    std::unique_ptr<MysteryMazeMode_MazeTable_KeyOwnerMapInfo> _keyOwnerMapInfo;
    std::map<int16_t, MysteryMazeMode_MazeTable_Quest> _keyOwnerQuests;
PLEASE CHECK IMPORTED TYPE ##gb::capnp::gamedata::CardCondition##
    std::vector<gb::capnp::gamedata::CardCondition> _keyOwnerCardConditions;
    std::vector<MysteryMazeMode_MazeTable_FollowerMapInfo> _followerMapInfos;
PLEASE CHECK IMPORTED TYPE ##gb::capnp::gamedata::CardCondition##
    std::vector<gb::capnp::gamedata::CardCondition> _followerCardConditions;
};

class MysteryMazeMode_MazeTable_FollowerMapInfo
{
public:
    MysteryMazeMode_MazeTable_FollowerMapInfo(gb::capnp::gamedata::MysteryMazeMode::MazeTable::FollowerMapInfo::Reader capnpReader);
    int64_t getPointThreshold() const { return _pointThreshold; }
    int32_t getMapId() const { return _mapId; }
    int32_t getMaxEntranceCount() const { return _maxEntranceCount; }

private:
    int64_t _pointThreshold;
    int32_t _mapId;
    int32_t _maxEntranceCount;
};

class MysteryMazeMode_MazeTable_KeyOwnerMapInfo
{
public:
    MysteryMazeMode_MazeTable_KeyOwnerMapInfo(gb::capnp::gamedata::MysteryMazeMode::MazeTable::KeyOwnerMapInfo::Reader capnpReader);
    int32_t getMapId() const { return _mapId; }
    int32_t getMaxEntranceCount() const { return _maxEntranceCount; }
    int64_t getPointJelliesCountLimit() const { return _pointJelliesCountLimit; }

private:
    int32_t _mapId;
    int32_t _maxEntranceCount;
    int64_t _pointJelliesCountLimit;
};

class MysteryMazeMode_MazeTable_Quest
{
public:
    MysteryMazeMode_MazeTable_Quest(gb::capnp::gamedata::MysteryMazeMode::MazeTable::Quest::Reader capnpReader);
    int16_t getId() const { return _id; }
    gb::capnp::gamedata::QuestGoal getGoal() const { return _goal; }
    std::string getGoalDescription() const { return _goalDescription; }

    const std::vector<gb::capnp::gamedata::Action>& getActions() const { return _actions; }
    const std::vector<gb::capnp::gamedata::QuestCondition>& getConditions() const { return _conditions; }
    const std::vector<gb::capnp::shared::ItemData>& getRewards() const { return _rewards; }
private:
    int16_t _id;
PLEASE CHECK IMPORTED TYPE ##gb::capnp::gamedata::QuestGoal##
    gb::capnp::gamedata::QuestGoal _goal;
    std::string _goalDescription;
PLEASE CHECK IMPORTED TYPE ##gb::capnp::gamedata::Action##
    std::vector<gb::capnp::gamedata::Action> _actions;
PLEASE CHECK IMPORTED TYPE ##gb::capnp::gamedata::QuestCondition##
    std::vector<gb::capnp::gamedata::QuestCondition> _conditions;
PLEASE CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
    std::vector<gb::capnp::shared::ItemData> _rewards;
};

class MysteryMazeMode_Season
{
public:
    MysteryMazeMode_Season(gb::capnp::gamedata::MysteryMazeMode::Season::Reader capnpReader);
    int16_t getId() const { return _id; }
    int64_t getStartTime() const { return _startTime; }
    int64_t getPlayEndTime() const { return _playEndTime; }
    int64_t getEndTime() const { return _endTime; }
    int16_t getSeasonRewardTableId() const { return _seasonRewardTableId; }
    int16_t getMazeTableId() const { return _mazeTableId; }

private:
    int16_t _id;
    int64_t _startTime;
    int64_t _playEndTime;
    int64_t _endTime;
    int16_t _seasonRewardTableId;
    int16_t _mazeTableId;
};

class MysteryMazeMode_SeasonRewardTable
{
public:
    MysteryMazeMode_SeasonRewardTable(gb::capnp::gamedata::MysteryMazeMode::SeasonRewardTable::Reader capnpReader);
    int16_t getId() const { return _id; }

    const std::vector<MysteryMazeMode_SeasonRewardTable_Row>& getRows() const { return _rows; }
private:
    int16_t _id;
    std::vector<MysteryMazeMode_SeasonRewardTable_Row> _rows;
};

class MysteryMazeMode_SeasonRewardTable_Row
{
public:
    MysteryMazeMode_SeasonRewardTable_Row(gb::capnp::gamedata::MysteryMazeMode::SeasonRewardTable::Row::Reader capnpReader);
    int16_t getOrder() const { return _order; }
    int64_t getSequentialRank() const { return _sequentialRank; }
    int64_t getPercentileRank() const { return _percentileRank; }
    std::string getMailTitleKey() const { return _mailTitleKey; }
    std::string getMailContentsKey() const { return _mailContentsKey; }
    int64_t getExpiry() const { return _expiry; }

    const std::vector<gb::capnp::shared::ItemData>& getItems() const { return _items; }
private:
    int16_t _order;
    int64_t _sequentialRank;
    int64_t _percentileRank;
    std::string _mailTitleKey;
    std::string _mailContentsKey;
    int64_t _expiry;
PLEASE CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
    std::vector<gb::capnp::shared::ItemData> _items;
};

NS_GB_GAMEDATA_END

#endif
