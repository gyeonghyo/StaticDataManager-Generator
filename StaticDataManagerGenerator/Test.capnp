@0x9025c129a3782f84;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("gb::capnp::gamedata");

using Go = import "../import/go.capnp";
$Go.package("gamedata");
$Go.import("github.com/devsisters/gb-server-game/capnp/gamedata");

using Data = import "../import/gamedata.capnp";
using GB = import "../import/gingerbread.capnp";

using Common = import "common.capnp";
using Shared = import "../shared/shared.capnp";
using ItemData = import "../shared/item.capnp".ItemData;

struct Guild $Data.root {
  config            @0 :GuildConfig;
  levelInfos        @1 :List(GuildLevelInfo);
  guildMarks        @2 :List(GuildMark);
  shopData          @3 :GuildShop;
  adventures        @4 :List(GuildAdventure);
  attendanceTables  @5 :List(GuildAttendanceTable);
  attendanceEvents  @6 :List(AttendanceEvent);

  struct AttendanceEvent {
    # NOTE(kyungmin): 출석 이벤트는 자정에 시작하고 종료되어야 한다. 안그러면 대박보상을 2명이 받는 등 보상이 꼬일 것이다.
    startTimestamp @0 :Int64;
    endTimestamp   @1 :Int64;
  }
}

struct GuildShop {
    slotTables                      @0: List(SlotTable);
    slotCandidateTables             @1: List(SlotCandidateTable);
    # normalSlots에 들어갈 후보군

    struct SlotTable {
        id                          @0: Int16 $GB.key;
        specialSlots                @1: List(SpecialSlot);
        normalSlots                 @2: List(NormalSlot);
        startTimestamp              @3 :Int64;
        # 판매 시작 날짜. 날짜는 다른 테이블과 겹치지않도록 한다
        endTimestamp                @4 :Int64;
        # 이 시간이 지나면 각 슬롯별로 시간이 남아있어도 판매를 안한다

        struct NormalSlot {
            id                  @0 :Int16 $GB.key;
            candidateTableId    @1 :Int16;
            minGuildLevel       @2 :Int32;
        }

        struct SpecialSlot {
            id              @0 :Int16 $GB.key;
            item            @1 :SlotItem;
            minGuildLevel   @2 :Int32;
        }
    }

    struct SlotCandidateTable {
        id              @0 :Int16 $GB.key;
        candidates      @1 :List(Candidate);

        struct Candidate {
            id          @0 :Int16 $GB.key;
            item        @1 :SlotItem;
            weight      @2 :Int32 $Data.serverOnly;
        }
    }

    struct SlotItem {
        item                    @0 :ItemData;
        price                   @1 :Common.Price;
        buyCoolDownSec          @2 :Int32;
        maxBuyCount             @3 :Int16;
    }
}

struct GuildAdventure {
  id                        @0 :Int32 $GB.key;
  duration                  @1 :Int32;
  partyCookieCount          @2 :Int32;
  recommendationInfo        @3 :List(Recommendation);
  minGuildLevel             @4 :Int32;
  resultPools               @5 :List(ResultPool);
  eventDuration             @6 :Int32;
  eventResultPools          @7 :List(ResultPool);
  events                    @8 :List(Event);
  name                      @9 :Text;
  imageName                 @10:Text;
  description               @11:Text;

  struct Event {
    startTimestamp          @0 :Int64;
    endTimestamp            @1 :Int64;
  }

  struct Recommendation {
    cookieId                        @0 :Int32;
    additionalHighWeightPerLevel    @1 :Int32;
    # 레벨당 중급 보상 대신 상급 보상을 받을 확률 가산량
    dialogue                        @2 :Text;
  }

  struct ResultPool {
    type                    @0 :Type $GB.key;
    weightPermil            @1 :Int32;
    rewards                 @2 :List(Rewards);
    affection               @3 :Int32;
    # 탐험에 참여한 쿠키 중 하나에 무작위로 부여
    guildExp                @4 :Int64;

    enum Type {
      high                  @0;
      mid                   @1;
      low                   @2;
    }

    struct Rewards {
      item                  @0 :List(ItemData);
      weight                @1 :Int32;
    }
  }
}

struct GuildConfig {
  guildCreatePrice              @0 :Common.Price;
  guildCreateMinLevel           @1 :Int16;
  # 길드 가입 x시간 이후 길드젤리 생성 등
  guildCreateCooldownSec        @2 :Int64;
  # 길드 해산 후 다음 생성까지 x초
  guildJoinCooldownSec          @3 :Int64;
  # 길드 탈퇴 후 길드에 가입하기까지 x초
  guildSearchMaxRowCount        @4 :Int32;
  # 길드 검색시 한 페이지에 보이는 최대 개수
  guildSearchMaxPageCount       @5 :Int32;
  # 길드 검색시 최대 페이지 수
  guildMaxInvitationCount       @6 :Int32;
  # 한 길드가 보낼 수 있는 초대 최대 갯수
  userMaxInvitationCount        @7 :Int32;
  # 한 유저가 받을 수 있는 초대 최대 갯수
  guildMaxPendingCount          @8 :Int32;
  # 한 길드에 가입 대기할 수 있는 최대 인원 수
  guildDescriptionMaxLength     @9 :Int32;
  # 길드 소개글 최대 길이. 한글, 한문, 일어 등은 한 글자를 2글자로 취급한다.
  guildNameMaxLength            @10 :Int32;
  attendMaxSeatId               @11 :Int32;
  # 출석 자리 갯수. 자리 ID(seatId)는 [1, attendMaxSeatId] 범위를 가진다.
  attendMessageMaxLength        @12 :Int32;
  # 출석 인사말 최대 길이. 한글, 한문, 일어 등은 한 글자를 2글자로 취급한다.
  attendExp                     @13 :Int64;
  # 1명 출석 당 받을 길드 경혐치, 일일 최대 attendExp*maxMemberCount
  guildRecommendationMaxCount   @14 :Int32;
  # 길드 검색시 목록에 보이는 최대 개수
  maxAdventurePartyCount        @15 :Int32;
  guildCreateTrophyList         @16 :List(Int32);
  # 길드 생성 시 표시할 가입조건 트로피 갯수 목록
  guildAttendMessageCooldownSec @17 :Int64;
}

struct GuildLevelInfo {
  level                   @0 :Int32 $GB.key;
  maxMemberCount          @1 :Int32;
  accExp                  @2 :Int64;
  attendanceTableId       @3 :Int32;
  attendanceEventTableId  @4 :Int32;
  # 출석 이벤트가 진행중일 때 적용할 보상 테이블
  benefitInfo             @5 :Text;
  benefitInfoSub          @6 :Text;
  guildMarkFrame          @7 :Text;
  benefitInfoSimple       @8 :Text;
}

struct GuildMark {
  id                @0 :Int32 $GB.key;
  resourceKey       @1 :Text;
  unlockCondition   @2 :GuildUnlockCondition;
}

struct GuildUnlockCondition {
  unlockType        @0 :Type;
  param             @1 :Int64;
  description       @2 :Text;

  enum Type {
    none                            @0;
    level                           @1;
   }
}

struct GuildAttendanceTable {
  id                   @0 :Int32 $GB.key;
  rewardCandidates     @1 :List(Reward) $Data.serverOnly;
  # 기본적으로 출석판에 깔아둘 보상 목록
  fallbackReward       @2 :List(ItemData) $Data.serverOnly;
  # 출석보상을 받고 탈퇴한 유저의 빈자리에 다른 유저가 출석을 하면 받을 보상

  giveawayMailTitle    @3 :Text;
  # 전체보상 우편 제목
  giveawayMailMessage  @4 :Text;
  # 전체보상 우편 내용(param 0: 출석한 유저 닉네임)
  giveawayMailExpiry   @5 :Int64;
  # 전체보상 우편 만료 시간(출석한 당시 시간 기준)

  struct Reward {
    id             @0 :Int32 $GB.key;
    minCount       @1 :Int16;
    # 한 출석판에 이 보상이 있을 최소 갯수. 모든 ItemBox의 minCount 합이 자릿수보다 많을 경우 오류를 내므로 유의
    maxCount       @2 :Int16;
    # 한 출석판에 이 보상이 있을 수 있는 최대 갯수. 아무리 weight를 높게 줘도 이 이상으로는 안들어간다.
    weight         @3 :Int32;
    # 모든 Reward들을 최소 보장 갯수(minCount)만큼 깔고 남은 자리를 채울 때, 이 보상이 나올 비중
    items          @4 :List(ItemData);
    # 이 보상 자리에 출석했으면 줄 보상 목록
    giveawayItems  @5 :List(ItemData);
    # 이 보상을 받으면 다른 유저들에게 뿌릴 보상(비워두면 주지 않음)
    grade          @6 :Shared.GuildAttendanceRewardGrade;
  }
}
