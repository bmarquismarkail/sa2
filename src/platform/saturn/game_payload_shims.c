#include "global.h"
#include "malloc_vram.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"

#include "game/game.h"
#include "game/globals.h"
#include "game/sa2/title_screen.h"
#include "game/shared/stage/camera.h"
#include "game/shared/stage/mp_event_mgr.h"
#include "game/shared/stage/mp_sprite_task.h"
#include "game/shared/stage/player.h"
#include "game/shared/stage/stage.h"
#include "game/shared/stage/water_effects.h"

#include "constants/animations.h"

#if PLATFORM_SATURN && SATURN_STAGE2_GAME_ONLY && (GAME == GAME_SA2)

static void SaturnGamePayloadReturnToTitle(void)
{
    CreateTitleScreen();
}

bool8 gShouldSpawnMPAttack2Effect = FALSE;

static const u16 sStageGoalScoreBonusAnimData[][3] = {
    { 36, SA2_ANIM_STAGE_GOAL_SCORE_BONUS, 1 }, { 36, SA2_ANIM_STAGE_GOAL_SCORE_BONUS, 2 },
    { 36, SA2_ANIM_STAGE_GOAL_SCORE_BONUS, 3 }, { 36, SA2_ANIM_STAGE_GOAL_SCORE_BONUS, 4 },
    { 36, SA2_ANIM_STAGE_GOAL_SCORE_BONUS, 5 },
};

struct Task *CreateMultiplayerSpriteTask(s16 x, s16 y, u8 param2, s8 param3, TaskMain main, TaskDestructor dtor)
{
    struct Task *t = TaskCreate(main, sizeof(MultiplayerSpriteTask), 0x4001, 0, dtor);
    MultiplayerSpriteTask *taskData;
    Sprite *s;

    if ((t == NULL) || (t == &gEmptyTask) || (TASK_DATA(t) == NULL)) {
        if ((t != NULL) && (t != &gEmptyTask)) {
            TaskDestroy(t);
        }

        return NULL;
    }

    taskData = TASK_DATA(t);
    s = &taskData->s;

    taskData->x = x;
    taskData->y = y;
    taskData->unk14 = param2;
    taskData->mpPlayerID = param3;
    taskData->playerAnim = 0;
    taskData->playerVariant = 0;

    s->graphics.dest = NULL;
    s->graphics.size = 0;
    s->graphics.anim = 0;
    s->variant = 0;
    s->prevVariant = -1;
    s->oamFlags = SPRITE_OAM_ORDER(0);
    s->qAnimDelay = 0;
    s->animSpeed = SPRITE_ANIM_SPEED(1.0);
    s->palId = 0;
    s->frameFlags = 0;

    return t;
}

void Task_UpdateMpSpriteTaskSprite(void)
{
    MultiplayerSpriteTask *taskData = TASK_DATA(gCurTask);
    Sprite *s = &taskData->s;

    if (!PLAYER_IS_ALIVE) {
        TaskDestroy(gCurTask);
        return;
    }

    if ((taskData->unk14 & 0x8) && ((taskData->playerAnim != gPlayer.anim) || (taskData->playerVariant != gPlayer.variant))) {
        TaskDestroy(gCurTask);
        return;
    }

    if (s->frameFlags & SPRITE_FLAG_MASK_ANIM_OVER) {
        TaskDestroy(gCurTask);
        return;
    }

    switch (taskData->unk14 & 0x3) {
        case 0:
            switch (taskData->unk14 & 0x30) {
                case 0x20: {
                    Player *p = GET_SP_PLAYER_V1(taskData->mpPlayerID);

                    taskData->x = I(p->qWorldX);
                    taskData->y = I(p->qWorldY);
                } break;

                case 0x10:
                    taskData->y = gWater.currentWaterLevel;
                    break;
            }

            s->x = taskData->x - gCamera.x;
            s->y = taskData->y - gCamera.y;
            break;

        case 1:
            s->x = taskData->x - gCamera.unk52;
            s->y = taskData->y - gCamera.unk54;
            break;

        case 2:
            s->x = taskData->x;
            s->y = taskData->y;
            break;
    }

    if (taskData->unk14 & 0x40) {
        if (!(gPlayer.moveState & MOVESTATE_FACING_LEFT)) {
            s->frameFlags |= SPRITE_FLAG_MASK_X_FLIP;
        } else {
            s->frameFlags &= ~SPRITE_FLAG_MASK_X_FLIP;
        }
    }

    if (taskData->unk14 & 0x80) {
        if (GRAVITY_IS_INVERTED) {
            s->frameFlags |= SPRITE_FLAG_MASK_Y_FLIP;
        } else {
            s->frameFlags &= ~SPRITE_FLAG_MASK_Y_FLIP;
        }
    }

    UpdateSpriteAnimation(s);
    DisplaySprite(s);
}

struct Task *CreateStageGoalBonusPointsAnim(s32 x, s32 y, u16 score)
{
    u16 bonusIndex;
    struct Task *t;
    MultiplayerSpriteTask *taskData;
    Sprite *s;

    switch (score) {
        case 100:
            bonusIndex = 0;
            break;

        case 200:
            bonusIndex = 1;
            break;

        case 300:
            bonusIndex = 2;
            break;

        case 500:
            bonusIndex = 3;
            break;

        case 800:
            bonusIndex = 4;
            break;

        default:
            return NULL;
    }

    t = CreateMultiplayerSpriteTask(x, y, 32, 0, Task_UpdateMpSpriteTaskSprite, TaskDestructor_MultiplayerSpriteTask);
    if (t == NULL) {
        return NULL;
    }

    taskData = TASK_DATA(t);
    s = &taskData->s;
    s->graphics.dest = VramMalloc(sStageGoalScoreBonusAnimData[bonusIndex][0]);
    if (s->graphics.dest == NULL) {
        TaskDestroy(t);
        return NULL;
    }

    s->graphics.anim = sStageGoalScoreBonusAnimData[bonusIndex][1];
    s->variant = sStageGoalScoreBonusAnimData[bonusIndex][2];
    s->oamFlags = SPRITE_OAM_ORDER(8);
    s->frameFlags = SPRITE_FLAG(PRIORITY, 2);

    return t;
}

void CreateGrindEffect2(void)
{
    Player *p = &gPlayer;
    s32 yOffset = p->spriteOffsetY;
    s32 x;
    s32 y;
    s32 x2;
    s32 y2;
    struct Task *t;
    MultiplayerSpriteTask *taskData;
    Sprite *s;

    if ((gStageTime & 0x7) != 0) {
        return;
    }

    if (GRAVITY_IS_INVERTED) {
        yOffset = -yOffset;
    }

    x = I(yOffset * SIN_24_8(p->rotation * 4));
    y = I(yOffset * COS_24_8(p->rotation * 4));
    x2 = I(p->qWorldX) - x;
    y2 = I(p->qWorldY) + y;

    t = CreateMultiplayerSpriteTask(x2, y2, 192, 0, Task_UpdateMpSpriteTaskSprite, TaskDestructor_MultiplayerSpriteTask);
    if (t == NULL) {
        return;
    }

    taskData = TASK_DATA(t);
    s = &taskData->s;
    s->graphics.dest = VramMalloc(20);
    if (s->graphics.dest == NULL) {
        TaskDestroy(t);
        return;
    }

    s->graphics.anim = SA2_ANIM_GRIND_EFFECT;
    s->variant = 0;
    s->oamFlags = SPRITE_OAM_ORDER(8);
    s->frameFlags = SPRITE_FLAG(PRIORITY, 1);
}

void TaskDestructor_MultiplayerSpriteTask(struct Task *t)
{
    MultiplayerSpriteTask *taskData = TASK_DATA(t);

    VramFree(taskData->s.graphics.dest);
}

void *CreateRoomEvent(void)
{
    RoomEvent *result = &gRoomEventQueue[gRoomEventQueueWritePos];

    gRoomEventQueueWritePos = (gRoomEventQueueWritePos + 1) & 0xF;

    return result;
}

struct Task *CreateMultiplayerReceiveEventMgr(void) { return NULL; }
struct Task *CreateMultiplayerSendEventMgr(void) { return NULL; }
void CreateMultiplayerPlayer(u8 id) { (void)id; }
void CreateOpponentPositionIndicator(u8 sid) { (void)sid; }
void CreateSelfPositionIndicator(void) { }
void CreateMultiplayerFinishResult(u8 sioId, u8 count)
{
    (void)sioId;
    (void)count;
}
void CreateMultiplayerFinishHandler(void) { }
void sub_8018818(void) { }

void CreateMultiplayerModeSelectScreen(void)
{
    SaturnGamePayloadReturnToTitle();
}

void LinkCommunicationError(void)
{
    SaturnGamePayloadReturnToTitle();
}

void CreateMultiplayerResultsScreen(u8 mode)
{
    (void)mode;
    SaturnGamePayloadReturnToTitle();
}

void CreateMultiplayerSinglePakResultsScreen(u32 a)
{
    (void)a;
    SaturnGamePayloadReturnToTitle();
}

void ShowSinglePakResults(void)
{
    SaturnGamePayloadReturnToTitle();
}

#endif
