//
//  gbGuildStaticDataManager.cpp
//  gbGame
//
//  Created by StaticDataManagerGenerator on 2019. 04. 29
//

#include "gbGuildStaticDataManager.h"
#include "gbCapnpHelper.h"

NS_GB_GAMEDATA_BEGIN

Guild::Guild()
{
    loadStaticData();
}

Guild::~Guild()
{
    unloadStaticData();
}

void Guild::loadStaticData()
{
    auto capnpReader = CapnpHelper::FileReader<gb::capnp::gamedata::Guild>("guild.dxc").getRoot();
     _config = std::make_unique<GuildConfig>(capnpReader.getConfig());
     _shopData = std::make_unique<GuildShop>(capnpReader.getShopData());
    for (auto capnp : capnpReader.getLevelInfos())
    {
        _levelInfos.emplace(capnp.getLevel(), GuildLevelInfo(capnp));
    }
    for (auto capnp : capnpReader.getGuildMarks())
    {
        _guildMarks.emplace(capnp.getId(), GuildMark(capnp));
    }
    for (auto capnp : capnpReader.getAdventures())
    {
        _adventures.emplace(capnp.getId(), GuildAdventure(capnp));
    }
    for (auto capnp : capnpReader.getAttendanceTables())
    {
        _attendanceTables.emplace(capnp.getId(), GuildAttendanceTable(capnp));
    }
    for (auto capnp : capnpReader.getAttendanceEvents())
    {
        _attendanceEvents.emplace_back(capnp);
    }
}

void Guild::unloadStaticData()
{
    _levelInfos.clear();
    _guildMarks.clear();
    _adventures.clear();
    _attendanceTables.clear();
    _attendanceEvents.clear();
}

GuildAdventure::GuildAdventure(gb::capnp::gamedata::GuildAdventure::Reader capnpReader)
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
        _recommendationInfo.emplace_back(capnp);
    }
    for (auto capnp : capnpReader.getResultPools())
    {
        _resultPools.emplace(capnp.getType, GuildAdventure_ResultPool(capnp));
    }
    for (auto capnp : capnpReader.getEventResultPools())
    {
        _eventResultPools.emplace(capnp.getType, GuildAdventure_ResultPool(capnp));
    }
    for (auto capnp : capnpReader.getEvents())
    {
        _events.emplace_back(capnp);
    }
}

GuildAdventure_Event::GuildAdventure_Event(gb::capnp::gamedata::GuildAdventure::Event::Reader capnpReader)
: _startTimestamp(capnpReader.getStartTimestamp())
, _endTimestamp(capnpReader.getEndTimestamp())
{
}

GuildAdventure_Recommendation::GuildAdventure_Recommendation(gb::capnp::gamedata::GuildAdventure::Recommendation::Reader capnpReader)
: _cookieId(capnpReader.getCookieId())
, _additionalHighWeightPerLevel(capnpReader.getAdditionalHighWeightPerLevel())
, _dialogue(capnpReader.getDialogue())
{
}

GuildAdventure_ResultPool::GuildAdventure_ResultPool(gb::capnp::gamedata::GuildAdventure::ResultPool::Reader capnpReader)
: _type(capnpReader.getType())
, _weightPermil(capnpReader.getWeightPermil())
, _affection(capnpReader.getAffection())
, _guildExp(capnpReader.getGuildExp())
{
    for (auto capnp : capnpReader.getRewards())
    {
        _rewards.emplace_back(capnp);
    }
}

GuildAdventure_ResultPool_Rewards::GuildAdventure_ResultPool_Rewards(gb::capnp::gamedata::GuildAdventure::ResultPool::Rewards::Reader capnpReader)
: _weight(capnpReader.getWeight())
{
PLEASE CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
    for (auto capnp : capnpReader.getItem())
    {
        _item.emplace_back(capnp);
    }
}

GuildAttendanceTable::GuildAttendanceTable(gb::capnp::gamedata::GuildAttendanceTable::Reader capnpReader)
: _id(capnpReader.getId())
, _giveawayMailTitle(capnpReader.getGiveawayMailTitle())
, _giveawayMailMessage(capnpReader.getGiveawayMailMessage())
, _giveawayMailExpiry(capnpReader.getGiveawayMailExpiry())
{
}

GuildAttendanceTable_Reward::GuildAttendanceTable_Reward(gb::capnp::gamedata::GuildAttendanceTable::Reward::Reader capnpReader)
: _id(capnpReader.getId())
, _minCount(capnpReader.getMinCount())
, _maxCount(capnpReader.getMaxCount())
, _weight(capnpReader.getWeight())
PLEASE CHECK IMPORTED TYPE ##gb::capnp::shared::GuildAttendanceRewardGrade##
, _grade(capnpReader.getGrade())
{
PLEASE CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
    for (auto capnp : capnpReader.getItems())
    {
        _items.emplace_back(capnp);
    }
PLEASE CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
    for (auto capnp : capnpReader.getGiveawayItems())
    {
        _giveawayItems.emplace_back(capnp);
    }
}

GuildConfig::GuildConfig(gb::capnp::gamedata::GuildConfig::Reader capnpReader)
PLEASE CHECK IMPORTED TYPE ##gb::capnp::gamedata::Price##
: _guildCreatePrice(capnpReader.getGuildCreatePrice())
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
        _guildCreateTrophyList.emplace_back(capnp);
    }
}

GuildLevelInfo::GuildLevelInfo(gb::capnp::gamedata::GuildLevelInfo::Reader capnpReader)
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

GuildMark::GuildMark(gb::capnp::gamedata::GuildMark::Reader capnpReader)
: _id(capnpReader.getId())
, _resourceKey(capnpReader.getResourceKey())
, _unlockCondition(std::make_unique<GuildUnlockCondition>(capnpReader.getUnlockCondition()))
{
}

GuildShop::GuildShop(gb::capnp::gamedata::GuildShop::Reader capnpReader)
{
    for (auto capnp : capnpReader.getSlotTables())
    {
        _slotTables.emplace(capnp.getId, GuildShop_SlotTable(capnp));
    }
    for (auto capnp : capnpReader.getSlotCandidateTables())
    {
        _slotCandidateTables.emplace(capnp.getId, GuildShop_SlotCandidateTable(capnp));
    }
}

GuildShop_SlotCandidateTable::GuildShop_SlotCandidateTable(gb::capnp::gamedata::GuildShop::SlotCandidateTable::Reader capnpReader)
: _id(capnpReader.getId())
{
    for (auto capnp : capnpReader.getCandidates())
    {
        _candidates.emplace(capnp.getId, GuildShop_SlotCandidateTable_Candidate(capnp));
    }
}

GuildShop_SlotCandidateTable_Candidate::GuildShop_SlotCandidateTable_Candidate(gb::capnp::gamedata::GuildShop::SlotCandidateTable::Candidate::Reader capnpReader)
: _id(capnpReader.getId())
, _item(std::make_unique<GuildShop_SlotItem>(capnpReader.getItem()))
{
}

GuildShop_SlotItem::GuildShop_SlotItem(gb::capnp::gamedata::GuildShop::SlotItem::Reader capnpReader)
PLEASE CHECK IMPORTED TYPE ##gb::capnp::shared::ItemData##
: _item(capnpReader.getItem())
PLEASE CHECK IMPORTED TYPE ##gb::capnp::gamedata::Price##
, _price(capnpReader.getPrice())
, _buyCoolDownSec(capnpReader.getBuyCoolDownSec())
, _maxBuyCount(capnpReader.getMaxBuyCount())
{
}

GuildShop_SlotTable::GuildShop_SlotTable(gb::capnp::gamedata::GuildShop::SlotTable::Reader capnpReader)
: _id(capnpReader.getId())
, _startTimestamp(capnpReader.getStartTimestamp())
, _endTimestamp(capnpReader.getEndTimestamp())
{
    for (auto capnp : capnpReader.getSpecialSlots())
    {
        _specialSlots.emplace(capnp.getId, GuildShop_SlotTable_SpecialSlot(capnp));
    }
    for (auto capnp : capnpReader.getNormalSlots())
    {
        _normalSlots.emplace(capnp.getId, GuildShop_SlotTable_NormalSlot(capnp));
    }
}

GuildShop_SlotTable_NormalSlot::GuildShop_SlotTable_NormalSlot(gb::capnp::gamedata::GuildShop::SlotTable::NormalSlot::Reader capnpReader)
: _id(capnpReader.getId())
, _candidateTableId(capnpReader.getCandidateTableId())
, _minGuildLevel(capnpReader.getMinGuildLevel())
{
}

GuildShop_SlotTable_SpecialSlot::GuildShop_SlotTable_SpecialSlot(gb::capnp::gamedata::GuildShop::SlotTable::SpecialSlot::Reader capnpReader)
: _id(capnpReader.getId())
, _item(std::make_unique<GuildShop_SlotItem>(capnpReader.getItem()))
, _minGuildLevel(capnpReader.getMinGuildLevel())
{
}

GuildUnlockCondition::GuildUnlockCondition(gb::capnp::gamedata::GuildUnlockCondition::Reader capnpReader)
: _unlockType(capnpReader.getUnlockType())
, _param(capnpReader.getParam())
, _description(capnpReader.getDescription())
{
}

Guild_AttendanceEvent::Guild_AttendanceEvent(gb::capnp::gamedata::Guild::AttendanceEvent::Reader capnpReader)
: _startTimestamp(capnpReader.getStartTimestamp())
, _endTimestamp(capnpReader.getEndTimestamp())
{
}

NS_GB_GAMEDATA_END
