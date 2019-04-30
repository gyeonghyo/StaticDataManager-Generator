//
//  gbGuildStaticDataManager.h
//  gbGame
//
//  Created by StaticDataManagerGenerator on 2019. 04. 30
//

#ifndef gbGuildStaticDataManager_h
#define gbGuildStaticDataManager_h

#include "gbStaticDataManager.h"
#include "capnp/gamedata/guild.capnp.h"
#include "gbMacros.h"

#define GuildStaticDataManagerInstance gb::gamedata::GuildStaticDataManager::getInstance()

NS_GB_GAMEDATA_SHOP_BEGIN
class Price;
NS_GB_GAMEDATA_SHOP_END

NS_GB_ITEM_BEGIN
class Item;
NS_GB_ITEM_END

NS_GB_GAMEDATA_BEGIN

class GuildAdventure_Event
{
public:
    GuildAdventure_Event(const gb::capnp::gamedata::GuildAdventure::Event::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int64_t, _startTimestamp, StartTimestamp);
    GB_SYNTHESIZE_READONLY(int64_t, _endTimestamp, EndTimestamp);
private:
};

class GuildAdventure_Recommendation
{
public:
    GuildAdventure_Recommendation(const gb::capnp::gamedata::GuildAdventure::Recommendation::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int32_t, _cookieId, CookieId);
    // 레벨당 중급 보상 대신 상급 보상을 받을 확률 가산량
    GB_SYNTHESIZE_READONLY(int32_t, _additionalHighWeightPerLevel, AdditionalHighWeightPerLevel);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _dialogue, Dialogue);
private:
};

class GuildAdventure_ResultPool_Rewards
{
public:
    GuildAdventure_ResultPool_Rewards(const gb::capnp::gamedata::GuildAdventure::ResultPool::Rewards::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int32_t, _weight, Weight);
CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::vector<std::unique_ptr<item::Item>>, _item, Item);
private:
};

class GuildAttendanceTable
{
public:
    GuildAttendanceTable(const gb::capnp::gamedata::GuildAttendanceTable::Reader& capnpReader);
    // 출석보상을 받고 탈퇴한 유저의 빈자리에 다른 유저가 출석을 하면 받을 보상
    GB_SYNTHESIZE_READONLY(int32_t, _id, Id);
    // 전체보상 우편 제목
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _giveawayMailTitle, GiveawayMailTitle);
    // 전체보상 우편 내용(param 0: 출석한 유저 닉네임)
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _giveawayMailMessage, GiveawayMailMessage);
    // 전체보상 우편 만료 시간(출석한 당시 시간 기준)
    GB_SYNTHESIZE_READONLY(int64_t, _giveawayMailExpiry, GiveawayMailExpiry);
private:
};

class GuildConfig
{
public:
    GuildConfig(const gb::capnp::gamedata::GuildConfig::Reader& capnpReader);
CHECK IMPORTED TYPE ##gb::capnp::gamedata::Price##
    shop::Price* getGuildCreatePrice() const { return _guildCreatePrice.get(); }
    // 길드 가입 x시간 이후 길드젤리 생성 등
    GB_SYNTHESIZE_READONLY(int16_t, _guildCreateMinLevel, GuildCreateMinLevel);
    // 길드 해산 후 다음 생성까지 x초
    GB_SYNTHESIZE_READONLY(int64_t, _guildCreateCooldownSec, GuildCreateCooldownSec);
    // 길드 탈퇴 후 길드에 가입하기까지 x초
    GB_SYNTHESIZE_READONLY(int64_t, _guildJoinCooldownSec, GuildJoinCooldownSec);
    // 길드 검색시 한 페이지에 보이는 최대 개수
    GB_SYNTHESIZE_READONLY(int32_t, _guildSearchMaxRowCount, GuildSearchMaxRowCount);
    // 길드 검색시 최대 페이지 수
    GB_SYNTHESIZE_READONLY(int32_t, _guildSearchMaxPageCount, GuildSearchMaxPageCount);
    // 한 길드가 보낼 수 있는 초대 최대 갯수
    GB_SYNTHESIZE_READONLY(int32_t, _guildMaxInvitationCount, GuildMaxInvitationCount);
    // 한 유저가 받을 수 있는 초대 최대 갯수
    GB_SYNTHESIZE_READONLY(int32_t, _userMaxInvitationCount, UserMaxInvitationCount);
    // 한 길드에 가입 대기할 수 있는 최대 인원 수
    GB_SYNTHESIZE_READONLY(int32_t, _guildMaxPendingCount, GuildMaxPendingCount);
    // 길드 소개글 최대 길이. 한글, 한문, 일어 등은 한 글자를 2글자로 취급한다.
    GB_SYNTHESIZE_READONLY(int32_t, _guildDescriptionMaxLength, GuildDescriptionMaxLength);
    GB_SYNTHESIZE_READONLY(int32_t, _guildNameMaxLength, GuildNameMaxLength);
    // 출석 자리 갯수. 자리 ID(seatId)는 [1, attendMaxSeatId] 범위를 가진다.
    GB_SYNTHESIZE_READONLY(int32_t, _attendMaxSeatId, AttendMaxSeatId);
    // 출석 인사말 최대 길이. 한글, 한문, 일어 등은 한 글자를 2글자로 취급한다.
    GB_SYNTHESIZE_READONLY(int32_t, _attendMessageMaxLength, AttendMessageMaxLength);
    // 1명 출석 당 받을 길드 경혐치, 일일 최대 attendExp*maxMemberCount
    GB_SYNTHESIZE_READONLY(int64_t, _attendExp, AttendExp);
    // 길드 검색시 목록에 보이는 최대 개수
    GB_SYNTHESIZE_READONLY(int32_t, _guildRecommendationMaxCount, GuildRecommendationMaxCount);
    GB_SYNTHESIZE_READONLY(int32_t, _maxAdventurePartyCount, MaxAdventurePartyCount);
    GB_SYNTHESIZE_READONLY(int64_t, _guildAttendMessageCooldownSec, GuildAttendMessageCooldownSec);
    // 길드 생성 시 표시할 가입조건 트로피 갯수 목록
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::vector<int32_t>, _guildCreateTrophyList, GuildCreateTrophyList);
private:
CHECK IMPORTED TYPE ##gb::capnp::gamedata::Price##
    std::unique_ptr<shop::Price> _guildCreatePrice;
};

class GuildLevelInfo
{
public:
    GuildLevelInfo(const gb::capnp::gamedata::GuildLevelInfo::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int32_t, _level, Level);
    GB_SYNTHESIZE_READONLY(int32_t, _maxMemberCount, MaxMemberCount);
    GB_SYNTHESIZE_READONLY(int64_t, _accExp, AccExp);
    GB_SYNTHESIZE_READONLY(int32_t, _attendanceTableId, AttendanceTableId);
    // 출석 이벤트가 진행중일 때 적용할 보상 테이블
    GB_SYNTHESIZE_READONLY(int32_t, _attendanceEventTableId, AttendanceEventTableId);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _benefitInfo, BenefitInfo);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _benefitInfoSub, BenefitInfoSub);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _guildMarkFrame, GuildMarkFrame);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _benefitInfoSimple, BenefitInfoSimple);
private:
};

class GuildShop_SlotItem
{
public:
    GuildShop_SlotItem(const gb::capnp::gamedata::GuildShop::SlotItem::Reader& capnpReader);
CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
    item::Item* getItem() const { return _item.get(); }
CHECK IMPORTED TYPE ##gb::capnp::gamedata::Price##
    shop::Price* getPrice() const { return _price.get(); }
    GB_SYNTHESIZE_READONLY(int32_t, _buyCoolDownSec, BuyCoolDownSec);
    GB_SYNTHESIZE_READONLY(int16_t, _maxBuyCount, MaxBuyCount);
private:
CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
    std::unique_ptr<item::Item> _item;
CHECK IMPORTED TYPE ##gb::capnp::gamedata::Price##
    std::unique_ptr<shop::Price> _price;
};

class GuildShop_SlotTable_NormalSlot
{
public:
    GuildShop_SlotTable_NormalSlot(const gb::capnp::gamedata::GuildShop::SlotTable::NormalSlot::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int16_t, _id, Id);
    GB_SYNTHESIZE_READONLY(int16_t, _candidateTableId, CandidateTableId);
    GB_SYNTHESIZE_READONLY(int32_t, _minGuildLevel, MinGuildLevel);
private:
};

class GuildUnlockCondition
{
public:
    GuildUnlockCondition(const gb::capnp::gamedata::GuildUnlockCondition::Reader& capnpReader);
    gb::capnp::gamedata::GuildUnlockCondition::Type getUnlockType() const { return _unlockType; }
    GB_SYNTHESIZE_READONLY(int64_t, _param, Param);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _description, Description);
private:
    gb::capnp::gamedata::GuildUnlockCondition::Type _unlockType;
};

// NOTE(kyungmin): 출석 이벤트는 자정에 시작하고 종료되어야 한다. 안그러면 대박보상을 2명이 받는 등 보상이 꼬일 것이다.
class Guild_AttendanceEvent
{
public:
    Guild_AttendanceEvent(const gb::capnp::gamedata::Guild::AttendanceEvent::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int64_t, _startTimestamp, StartTimestamp);
    GB_SYNTHESIZE_READONLY(int64_t, _endTimestamp, EndTimestamp);
private:
};

class GuildAdventure_ResultPool
{
public:
    GuildAdventure_ResultPool(const gb::capnp::gamedata::GuildAdventure::ResultPool::Reader& capnpReader);
    gb::capnp::gamedata::GuildAdventure::ResultPool::Type getType() const { return _type; }
    GB_SYNTHESIZE_READONLY(int32_t, _weightPermil, WeightPermil);
    // 탐험에 참여한 쿠키 중 하나에 무작위로 부여
    GB_SYNTHESIZE_READONLY(int32_t, _affection, Affection);
    GB_SYNTHESIZE_READONLY(int64_t, _guildExp, GuildExp);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::vector<std::shared_ptr<GuildAdventure_ResultPool_Rewards>>, _rewards, Rewards);
private:
    gb::capnp::gamedata::GuildAdventure::ResultPool::Type _type;
};

class GuildShop_SlotCandidateTable_Candidate
{
public:
    GuildShop_SlotCandidateTable_Candidate(const gb::capnp::gamedata::GuildShop::SlotCandidateTable::Candidate::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int16_t, _id, Id);
    const GuildShop_SlotItem* getItem() const { return _item.get(); }
private:
    std::unique_ptr<GuildShop_SlotItem> _item;
};

class GuildShop_SlotTable_SpecialSlot
{
public:
    GuildShop_SlotTable_SpecialSlot(const gb::capnp::gamedata::GuildShop::SlotTable::SpecialSlot::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int16_t, _id, Id);
    const GuildShop_SlotItem* getItem() const { return _item.get(); }
    GB_SYNTHESIZE_READONLY(int32_t, _minGuildLevel, MinGuildLevel);
private:
    std::unique_ptr<GuildShop_SlotItem> _item;
};

class GuildMark
{
public:
    GuildMark(const gb::capnp::gamedata::GuildMark::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int32_t, _id, Id);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _resourceKey, ResourceKey);
    const GuildUnlockCondition* getUnlockCondition() const { return _unlockCondition.get(); }
private:
    std::unique_ptr<GuildUnlockCondition> _unlockCondition;
};

class GuildAdventure
{
public:
    GuildAdventure(const gb::capnp::gamedata::GuildAdventure::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int32_t, _id, Id);
    GB_SYNTHESIZE_READONLY(int32_t, _duration, Duration);
    GB_SYNTHESIZE_READONLY(int32_t, _partyCookieCount, PartyCookieCount);
    GB_SYNTHESIZE_READONLY(int32_t, _minGuildLevel, MinGuildLevel);
    GB_SYNTHESIZE_READONLY(int32_t, _eventDuration, EventDuration);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _name, Name);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _imageName, ImageName);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _description, Description);
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::vector<std::shared_ptr<GuildAdventure_Recommendation>>, _recommendationInfo, RecommendationInfo);
    const std::map<gb::capnp::gamedata::GuildAdventure::ResultPool::Type, std::shared_ptr<GuildAdventure_ResultPool>>& getAllResultPools() const { return _resultPools; }
    const GuildAdventure_ResultPool* getResultPool(const gb::capnp::gamedata::GuildAdventure::ResultPool::Type id) const;
    const std::map<gb::capnp::gamedata::GuildAdventure::ResultPool::Type, std::shared_ptr<GuildAdventure_ResultPool>>& getAllEventResultPools() const { return _eventResultPools; }
    const GuildAdventure_ResultPool* getEventResultPool(const gb::capnp::gamedata::GuildAdventure::ResultPool::Type id) const;
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::vector<std::shared_ptr<GuildAdventure_Event>>, _events, Events);
private:
    std::map<gb::capnp::gamedata::GuildAdventure::ResultPool::Type, std::shared_ptr<GuildAdventure_ResultPool>> _resultPools;
    std::map<gb::capnp::gamedata::GuildAdventure::ResultPool::Type, std::shared_ptr<GuildAdventure_ResultPool>> _eventResultPools;
};

class GuildShop_SlotCandidateTable
{
public:
    GuildShop_SlotCandidateTable(const gb::capnp::gamedata::GuildShop::SlotCandidateTable::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int16_t, _id, Id);
    const std::map<int16_t, std::shared_ptr<GuildShop_SlotCandidateTable_Candidate>>& getAllCandidates() const { return _candidates; }
    const GuildShop_SlotCandidateTable_Candidate* getCandidate(const int16_t id) const;
private:
    std::map<int16_t, std::shared_ptr<GuildShop_SlotCandidateTable_Candidate>> _candidates;
};

class GuildShop_SlotTable
{
public:
    GuildShop_SlotTable(const gb::capnp::gamedata::GuildShop::SlotTable::Reader& capnpReader);
    GB_SYNTHESIZE_READONLY(int16_t, _id, Id);
    // 판매 시작 날짜. 날짜는 다른 테이블과 겹치지않도록 한다
    GB_SYNTHESIZE_READONLY(int64_t, _startTimestamp, StartTimestamp);
    // 이 시간이 지나면 각 슬롯별로 시간이 남아있어도 판매를 안한다
    GB_SYNTHESIZE_READONLY(int64_t, _endTimestamp, EndTimestamp);
    const std::map<int16_t, std::shared_ptr<GuildShop_SlotTable_SpecialSlot>>& getAllSpecialSlots() const { return _specialSlots; }
    const GuildShop_SlotTable_SpecialSlot* getSpecialSlot(const int16_t id) const;
    const std::map<int16_t, std::shared_ptr<GuildShop_SlotTable_NormalSlot>>& getAllNormalSlots() const { return _normalSlots; }
    const GuildShop_SlotTable_NormalSlot* getNormalSlot(const int16_t id) const;
private:
    std::map<int16_t, std::shared_ptr<GuildShop_SlotTable_SpecialSlot>> _specialSlots;
    std::map<int16_t, std::shared_ptr<GuildShop_SlotTable_NormalSlot>> _normalSlots;
};

class GuildShop
{
public:
    GuildShop(const gb::capnp::gamedata::GuildShop::Reader& capnpReader);
    const std::map<int16_t, std::shared_ptr<GuildShop_SlotTable>>& getAllSlotTables() const { return _slotTables; }
    const GuildShop_SlotTable* getSlotTable(const int16_t id) const;
    // normalSlots에 들어갈 후보군
    const std::map<int16_t, std::shared_ptr<GuildShop_SlotCandidateTable>>& getAllSlotCandidateTables() const { return _slotCandidateTables; }
    const GuildShop_SlotCandidateTable* getSlotCandidateTable(const int16_t id) const;
private:
    std::map<int16_t, std::shared_ptr<GuildShop_SlotTable>> _slotTables;
    std::map<int16_t, std::shared_ptr<GuildShop_SlotCandidateTable>> _slotCandidateTables;
};

class GuildStaticDataManager 
: public StaticDataManager<GuildStaticDataManager>
{
public:
    GuildStaticDataManager();
    ~GuildStaticDataManager();
    const GuildConfig* getConfig() const { return _config.get(); }
    const GuildShop* getShopData() const { return _shopData.get(); }
    const std::map<int32_t, std::shared_ptr<GuildLevelInfo>>& getAllLevelInfos() const { return _levelInfos; }
    const GuildLevelInfo* getLevelInfo(const int32_t id) const;
    const std::map<int32_t, std::shared_ptr<GuildMark>>& getAllGuildMarks() const { return _guildMarks; }
    const GuildMark* getGuildMark(const int32_t id) const;
    const std::map<int32_t, std::shared_ptr<GuildAdventure>>& getAllAdventures() const { return _adventures; }
    const GuildAdventure* getAdventure(const int32_t id) const;
    const std::map<int32_t, std::shared_ptr<GuildAttendanceTable>>& getAllAttendanceTables() const { return _attendanceTables; }
    const GuildAttendanceTable* getAttendanceTable(const int32_t id) const;
    GB_SYNTHESIZE_READONLY_PASS_BY_REF(std::vector<std::shared_ptr<Guild_AttendanceEvent>>, _attendanceEvents, AttendanceEvents);
protected:
    void loadStaticData() override;
    void unloadStaticData() override;

private:
    std::unique_ptr<GuildConfig> _config;
    std::unique_ptr<GuildShop> _shopData;
    std::map<int32_t, std::shared_ptr<GuildLevelInfo>> _levelInfos;
    std::map<int32_t, std::shared_ptr<GuildMark>> _guildMarks;
    std::map<int32_t, std::shared_ptr<GuildAdventure>> _adventures;
    std::map<int32_t, std::shared_ptr<GuildAttendanceTable>> _attendanceTables;
};

NS_GB_GAMEDATA_END

#endif
