//
//  gbGuildModeStaticDataManager.cpp
//  gbGame
//
//  Created by StaticDataManagerGenerator on 2019. 04. 30
//

#include "gbGuildModeStaticDataManager.h"
#include "gbCapnpHelper.h"
#include "gbItemCapnpHelper.h"
#include "gbPriceCapnpHelper.h"

NS_GB_GAMEDATA_BEGIN

GuildModeStaticDataManager::GuildModeStaticDataManager()
{
    loadStaticData();
}

GuildModeStaticDataManager::~GuildModeStaticDataManager()
{
    unloadStaticData();
}

void GuildModeStaticDataManager::loadStaticData()
{
    auto capnpReader = CapnpHelper::FileReader<gb::capnp::gamedata::Guild>("guild.dxc").getRoot();
     _config = std::make_unique<GuildConfig>(capnpReader.getConfig());
     _shopData = std::make_unique<GuildShop>(capnpReader.getShopData());
    for (auto capnp : capnpReader.getLevelInfos())
    {
        _levelInfos.emplace(capnp.getLevel(), std::make_shared<GuildLevelInfo>(capnp));
    }
    for (auto capnp : capnpReader.getGuildMarks())
    {
        _guildMarks.emplace(capnp.getId(), std::make_shared<GuildMark>(capnp));
    }
    for (auto capnp : capnpReader.getAdventures())
    {
        _adventures.emplace(capnp.getId(), std::make_shared<GuildAdventure>(capnp));
    }
    for (auto capnp : capnpReader.getAttendanceTables())
    {
        _attendanceTables.emplace(capnp.getId(), std::make_shared<GuildAttendanceTable>(capnp));
    }
    for (auto capnp : capnpReader.getAttendanceEvents())
    {
        _attendanceEvents.emplace_back(std::make_shared<Guild_AttendanceEvent>(capnp));
    }
}

void GuildModeStaticDataManager::unloadStaticData()
{
    _levelInfos.clear();
    _guildMarks.clear();
    _adventures.clear();
    _attendanceTables.clear();
    _attendanceEvents.clear();
}

GuildAdventure_Event::GuildAdventure_Event(const gb::capnp::gamedata::GuildAdventure::Event::Reader& capnpReader)
: _startTimestamp(capnpReader.getStartTimestamp())
, _endTimestamp(capnpReader.getEndTimestamp())
{
}

GuildAdventure_Recommendation::GuildAdventure_Recommendation(const gb::capnp::gamedata::GuildAdventure::Recommendation::Reader& capnpReader)
: _cookieId(capnpReader.getCookieId())
, _additionalHighWeightPerLevel(capnpReader.getAdditionalHighWeightPerLevel())
, _dialogue(capnpReader.getDialogue())
{
}

GuildAdventure_ResultPool_Rewards::GuildAdventure_ResultPool_Rewards(const gb::capnp::gamedata::GuildAdventure::ResultPool::Rewards::Reader& capnpReader)
: _weight(capnpReader.getWeight())
{

    for (auto capnp : capnpReader.getItem())
    {
        _item.emplace_back(item::ItemCapnpHelper::makeItem(capnp));
    }
}

GuildAttendanceTable::GuildAttendanceTable(const gb::capnp::gamedata::GuildAttendanceTable::Reader& capnpReader)
: _id(capnpReader.getId())
, _giveawayMailTitle(capnpReader.getGiveawayMailTitle())
, _giveawayMailMessage(capnpReader.getGiveawayMailMessage())
, _giveawayMailExpiry(capnpReader.getGiveawayMailExpiry())
{
}

GuildConfig::GuildConfig(const gb::capnp::gamedata::GuildConfig::Reader& capnpReader)

: _guildCreatePrice(shop::PriceCapnpHelper::makePrice(capnpReader.getGuildCreatePrice()))
, _guildCreateMinLevel(capnpReader.getGuildCreateMinLevel())
, _guildCreateCooldownSec(capnpReader.getGuildCreateCooldownSec())
, _guildJoinCooldownSec(capnpReader.getGuildJoinCooldownSec())
, _guildSearchMaxRowCount(capnpReader.getGuildSearchMaxRowCount())
, _guildSearchMaxPageCount(capnpReader.getGuildSearchMaxPageCount())
, _guildMaxInvitationCount(capnpReader.getGuildMaxInvitationCount())
, _userMaxInvitationCount(capnpReader.getUserMaxInvitationCount())
, _guildMaxPendingCount(capnpReader.getGuildMaxPendingCount())
, _guildDescriptionMaxLength(capnpReader.getGuildDescriptionMaxLength())
, _guildNameMaxLength(capnpReader.getGuildNameMaxLength())
, _attendMaxSeatId(capnpReader.getAttendMaxSeatId())
, _attendMessageMaxLength(capnpReader.getAttendMessageMaxLength())
, _attendExp(capnpReader.getAttendExp())
, _guildRecommendationMaxCount(capnpReader.getGuildRecommendationMaxCount())
, _maxAdventurePartyCount(capnpReader.getMaxAdventurePartyCount())
, _guildAttendMessageCooldownSec(capnpReader.getGuildAttendMessageCooldownSec())
{
    for (auto capnp : capnpReader.getGuildCreateTrophyList())
    {
        _guildCreateTrophyList.emplace_back((capnp));
    }
}

GuildLevelInfo::GuildLevelInfo(const gb::capnp::gamedata::GuildLevelInfo::Reader& capnpReader)
: _level(capnpReader.getLevel())
, _maxMemberCount(capnpReader.getMaxMemberCount())
, _accExp(capnpReader.getAccExp())
, _attendanceTableId(capnpReader.getAttendanceTableId())
, _attendanceEventTableId(capnpReader.getAttendanceEventTableId())
, _benefitInfo(capnpReader.getBenefitInfo())
, _benefitInfoSub(capnpReader.getBenefitInfoSub())
, _guildMarkFrame(capnpReader.getGuildMarkFrame())
, _benefitInfoSimple(capnpReader.getBenefitInfoSimple())
{
}

GuildShop_SlotItem::GuildShop_SlotItem(const gb::capnp::gamedata::GuildShop::SlotItem::Reader& capnpReader)

: _item(item::ItemCapnpHelper::makeItem(capnpReader.getItem()))

, _price(shop::PriceCapnpHelper::makePrice(capnpReader.getPrice()))
, _buyCoolDownSec(capnpReader.getBuyCoolDownSec())
, _maxBuyCount(capnpReader.getMaxBuyCount())
{
}

GuildShop_SlotTable_NormalSlot::GuildShop_SlotTable_NormalSlot(const gb::capnp::gamedata::GuildShop::SlotTable::NormalSlot::Reader& capnpReader)
: _id(capnpReader.getId())
, _candidateTableId(capnpReader.getCandidateTableId())
, _minGuildLevel(capnpReader.getMinGuildLevel())
{
}

GuildUnlockCondition::GuildUnlockCondition(const gb::capnp::gamedata::GuildUnlockCondition::Reader& capnpReader)
: _unlockType(capnpReader.getUnlockType())
, _param(capnpReader.getParam())
, _description(capnpReader.getDescription())
{
}

Guild_AttendanceEvent::Guild_AttendanceEvent(const gb::capnp::gamedata::Guild::AttendanceEvent::Reader& capnpReader)
: _startTimestamp(capnpReader.getStartTimestamp())
, _endTimestamp(capnpReader.getEndTimestamp())
{
}

GuildAdventure_ResultPool::GuildAdventure_ResultPool(const gb::capnp::gamedata::GuildAdventure::ResultPool::Reader& capnpReader)
: _type(capnpReader.getType())
, _weightPermil(capnpReader.getWeightPermil())
, _affection(capnpReader.getAffection())
, _guildExp(capnpReader.getGuildExp())
{
    for (auto capnp : capnpReader.getRewards())
    {
        _rewards.emplace_back(std::make_shared<GuildAdventure_ResultPool_Rewards>(capnp));
    }
}

GuildShop_SlotCandidateTable_Candidate::GuildShop_SlotCandidateTable_Candidate(const gb::capnp::gamedata::GuildShop::SlotCandidateTable::Candidate::Reader& capnpReader)
: _id(capnpReader.getId())
, _item(std::make_unique<GuildShop_SlotItem>(capnpReader.getItem()))
{
}

GuildShop_SlotTable_SpecialSlot::GuildShop_SlotTable_SpecialSlot(const gb::capnp::gamedata::GuildShop::SlotTable::SpecialSlot::Reader& capnpReader)
: _id(capnpReader.getId())
, _item(std::make_unique<GuildShop_SlotItem>(capnpReader.getItem()))
, _minGuildLevel(capnpReader.getMinGuildLevel())
{
}

GuildMark::GuildMark(const gb::capnp::gamedata::GuildMark::Reader& capnpReader)
: _id(capnpReader.getId())
, _resourceKey(capnpReader.getResourceKey())
, _unlockCondition(std::make_unique<GuildUnlockCondition>(capnpReader.getUnlockCondition()))
{
}

GuildAdventure::GuildAdventure(const gb::capnp::gamedata::GuildAdventure::Reader& capnpReader)
: _id(capnpReader.getId())
, _duration(capnpReader.getDuration())
, _partyCookieCount(capnpReader.getPartyCookieCount())
, _minGuildLevel(capnpReader.getMinGuildLevel())
, _eventDuration(capnpReader.getEventDuration())
, _name(capnpReader.getName())
, _imageName(capnpReader.getImageName())
, _description(capnpReader.getDescription())
{
    for (auto capnp : capnpReader.getRecommendationInfo())
    {
        _recommendationInfo.emplace_back(std::make_shared<GuildAdventure_Recommendation>(capnp));
    }
    for (auto capnp : capnpReader.getResultPools())
    {
        _resultPools.emplace(capnp.getType(), std::make_shared<GuildAdventure_ResultPool>(capnp));
    }
    for (auto capnp : capnpReader.getEventResultPools())
    {
        _eventResultPools.emplace(capnp.getType(), std::make_shared<GuildAdventure_ResultPool>(capnp));
    }
    for (auto capnp : capnpReader.getEvents())
    {
        _events.emplace_back(std::make_shared<GuildAdventure_Event>(capnp));
    }
}

GuildShop_SlotCandidateTable::GuildShop_SlotCandidateTable(const gb::capnp::gamedata::GuildShop::SlotCandidateTable::Reader& capnpReader)
: _id(capnpReader.getId())
{
    for (auto capnp : capnpReader.getCandidates())
    {
        _candidates.emplace(capnp.getId(), std::make_shared<GuildShop_SlotCandidateTable_Candidate>(capnp));
    }
}

GuildShop_SlotTable::GuildShop_SlotTable(const gb::capnp::gamedata::GuildShop::SlotTable::Reader& capnpReader)
: _id(capnpReader.getId())
, _startTimestamp(capnpReader.getStartTimestamp())
, _endTimestamp(capnpReader.getEndTimestamp())
{
    for (auto capnp : capnpReader.getSpecialSlots())
    {
        _specialSlots.emplace(capnp.getId(), std::make_shared<GuildShop_SlotTable_SpecialSlot>(capnp));
    }
    for (auto capnp : capnpReader.getNormalSlots())
    {
        _normalSlots.emplace(capnp.getId(), std::make_shared<GuildShop_SlotTable_NormalSlot>(capnp));
    }
}

GuildShop::GuildShop(const gb::capnp::gamedata::GuildShop::Reader& capnpReader)
{
    for (auto capnp : capnpReader.getSlotTables())
    {
        _slotTables.emplace(capnp.getId(), std::make_shared<GuildShop_SlotTable>(capnp));
    }
    for (auto capnp : capnpReader.getSlotCandidateTables())
    {
        _slotCandidateTables.emplace(capnp.getId(), std::make_shared<GuildShop_SlotCandidateTable>(capnp));
    }
}

const GuildAdventure_ResultPool* GuildAdventure::getResultPool(const gb::capnp::gamedata::GuildAdventure::ResultPool::Type id) const
{
    auto it = _resultPools.find(id);
    return it != _resultPools.end() ? it->second.get() : nullptr;
}
const GuildAdventure_ResultPool* GuildAdventure::getEventResultPool(const gb::capnp::gamedata::GuildAdventure::ResultPool::Type id) const
{
    auto it = _eventResultPools.find(id);
    return it != _eventResultPools.end() ? it->second.get() : nullptr;
}
const GuildShop_SlotCandidateTable_Candidate* GuildShop_SlotCandidateTable::getCandidate(const int16_t id) const
{
    auto it = _candidates.find(id);
    return it != _candidates.end() ? it->second.get() : nullptr;
}
const GuildShop_SlotTable_SpecialSlot* GuildShop_SlotTable::getSpecialSlot(const int16_t id) const
{
    auto it = _specialSlots.find(id);
    return it != _specialSlots.end() ? it->second.get() : nullptr;
}
const GuildShop_SlotTable_NormalSlot* GuildShop_SlotTable::getNormalSlot(const int16_t id) const
{
    auto it = _normalSlots.find(id);
    return it != _normalSlots.end() ? it->second.get() : nullptr;
}
const GuildShop_SlotTable* GuildShop::getSlotTable(const int16_t id) const
{
    auto it = _slotTables.find(id);
    return it != _slotTables.end() ? it->second.get() : nullptr;
}
const GuildShop_SlotCandidateTable* GuildShop::getSlotCandidateTable(const int16_t id) const
{
    auto it = _slotCandidateTables.find(id);
    return it != _slotCandidateTables.end() ? it->second.get() : nullptr;
}
const GuildLevelInfo* GuildModeStaticDataManager::getLevelInfo(const int32_t id) const
{
    auto it = _levelInfos.find(id);
    return it != _levelInfos.end() ? it->second.get() : nullptr;
}
const GuildMark* GuildModeStaticDataManager::getGuildMark(const int32_t id) const
{
    auto it = _guildMarks.find(id);
    return it != _guildMarks.end() ? it->second.get() : nullptr;
}
const GuildAdventure* GuildModeStaticDataManager::getAdventure(const int32_t id) const
{
    auto it = _adventures.find(id);
    return it != _adventures.end() ? it->second.get() : nullptr;
}
const GuildAttendanceTable* GuildModeStaticDataManager::getAttendanceTable(const int32_t id) const
{
    auto it = _attendanceTables.find(id);
    return it != _attendanceTables.end() ? it->second.get() : nullptr;
}


NS_GB_GAMEDATA_END
