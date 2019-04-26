@0xf51afffbc6fa695a;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("gb::capnp::gamedata");

using Go = import "../import/go.capnp";
$Go.package("gamedata");
$Go.import("github.com/devsisters/gb-server-game/capnp/gamedata");

using Data = import "../import/gamedata.capnp";
using GB = import "../import/gingerbread.capnp";
using Action = import "achievements.capnp".Action;
using ItemData = import "../shared/item.capnp".ItemData;
using CardCondition = import "../gamedata/common.capnp".CardCondition;

using QuestGoal = import "quests.capnp".QuestGoal;
using QuestCondition = import "quests.capnp".QuestCondition;

# 챔피언스 리그에서 일정 등위이상 들었을 때 받는 열쇠를 사용하는 모드
# Key Owner Group의 성과에 따라 Follower Group의 플레이에 영향이 생기는 모드
# Key Owner Group에는 Quest, 탑 랭킹이 존재
# 매일 입장가능 횟수가 갱신됨
struct MysteryMazeMode $Data.root {
  seasons                   @0 :List(Season);
  seasonRewardTables        @1 :List(SeasonRewardTable);
  mazeTables                @2 :List(MazeTable);
  seasonRewardCalcDuration  @3 :Int64;
  # 시즌 종료 후 정산하는데 쓸 여유 시간

  struct Season {
    id                   @0 :Int16 $GB.key;
    # do not use less than or equal to 0
    startTime            @1 :Int64;
    playEndTime          @2 :Int64;
    # 플레이 종료하고 정산받을 수 있는 시간
    endTime              @3 :Int64;
    # 메뉴에서 완전히 사라지는 시간
    seasonRewardTableId  @4 :Int16;
    mazeTableId          @5 :Int16;
  }

  struct SeasonRewardTable {
    id    @0 :Int16 $GB.key;
    rows  @1 :List(Row);

    struct Row {
      order            @0 :Int16;
      # 숫자가 낮을 수록 좋은 보상이라고 가정한다.
      sequentialRank   @1 :Int64;
      percentileRank   @2 :Int64;
      # sequential, percentile 둘 중 하나를 만족하면 지급한다.
      # 0보다 작으면 절대 만족시킬 수 없다.
      # percentileRank는 1000배 scale up 된다.
      # (ex: 상위 10.5% -> 10500)
      items            @3 :List(ItemData);
      mailTitleKey     @4 :Text;
      mailContentsKey  @5 :Text;
      expiry           @6 :Int64;
    }
  }

  struct MazeTable {
    id                      @0 :Int16 $GB.key;
    pointJellyId            @1 :Int32;
    # 원석 젤리 Id. 리소스 키는 jellyStat에서 찾는다.
    keyOwnerQuests          @2 :List(Quest);
    keyOwnerMapInfo         @3 :KeyOwnerMapInfo;
    keyOwnerCardConditions  @4 :List(CardCondition);
    followerMapInfos        @5 :List(FollowerMapInfo);
    followerCardConditions  @6 :List(CardCondition);
    # threshold가  "0"인 info가 1개 이상 존재해야 한다. 좋은 맵이 앞에 나오도록 작성

    struct Quest {
      id               @0 :Int16 $GB.key;
      actions          @1 :List(Action);
      conditions       @2 :List(QuestCondition);
      goal             @3 :QuestGoal;
      rewards          @4 :List(ItemData);
      goalDescription  @5 :Text;
    }

    struct KeyOwnerMapInfo {
      mapId                   @0 :Int32;
      maxEntranceCount        @1 :Int32;
      pointJelliesCountLimit  @2 :Int64;
      scoreSoftLimit          @3 :Int64 $Data.serverOnly;
      scoreHardLimit          @4 :Int64 $Data.serverOnly;
      itemSoftLimits          @5 :List(ItemData) $Data.serverOnly;
      itemHardLimits          @6 :List(ItemData) $Data.serverOnly;
      playTimeSoftLimit       @7 :Int64 $Data.serverOnly;
      playTimeHardLimit       @8 :Int64 $Data.serverOnly;
      coinSoftLimit           @9 :Int64 $Data.serverOnly;
      coinHardLimit           @10 :Int64 $Data.serverOnly;
      # 어뷰징 체크용.
    }

    struct FollowerMapInfo {
      pointThreshold     @0 :Int64;
      mapId              @1 :Int32;
      maxEntranceCount   @2 :Int32;
      scoreSoftLimit     @3 :Int64 $Data.serverOnly;
      scoreHardLimit     @4 :Int64 $Data.serverOnly;
      itemSoftLimits     @5 :List(ItemData) $Data.serverOnly;
      itemHardLimits     @6 :List(ItemData) $Data.serverOnly;
      playTimeSoftLimit  @7 :Int64 $Data.serverOnly;
      playTimeHardLimit  @8 :Int64 $Data.serverOnly;
      coinSoftLimit      @9 :Int64 $Data.serverOnly;
      coinHardLimit      @10 :Int64 $Data.serverOnly;
      # 어뷰징 체크용.
    }
  }
}
