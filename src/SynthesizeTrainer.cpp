#include "ScriptMgr.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "Chat.h"

class CreatureScript_SynthesizeTrainer : public CreatureScript
{
public:
    CreatureScript_SynthesizeTrainer() : CreatureScript("synthesize_trainer") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/spell_holy_avenginewrath:23:23:1:0|t 职业技能", 1, 1);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/Icons/inv_qiraj_jewelglyphed:23:23:1:0|t 专业技能", 1, 2);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/ability_mount_ridinghorse:23:23:1:0|t 骑术技能", 1, 3);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/Icons/Ability_DualWield:23:23:1:0|t 武器技能", 1, 4);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Mage_LivingBomb:23:23|t 开双天赋", 1, 5);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Achievement_WorldEvent_Lunar:23:23|t 重置天赋", 1, 6, "你确定要重置天赋吗？", 0, false);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Relics_IdolofFerocity:23:23|t 重置宠物天赋", 1, 8, "你确定要重置宠物天赋吗？", 0, false);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Rune_01:23:23|t 炉石绑定", 1, 7);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 Sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (Sender == 1)
        {
            if (action == 1) {
                uint32 npcspellid = 0;
                switch (player->getClass())
                {
                case CLASS_WARRIOR: npcspellid = 3042; break;
                case CLASS_PALADIN:   player->GetTeamId() == TEAM_HORDE ? npcspellid = 16679 : npcspellid = 16761; break;
                case CLASS_HUNTER:    npcspellid = 3352; break;
                case CLASS_ROGUE:    npcspellid = 3328; break;
                case CLASS_PRIEST:    npcspellid = 6014; break;
                case CLASS_DEATH_KNIGHT:    npcspellid = 28472; break;
                case CLASS_SHAMAN:    npcspellid = 3344; break;
                case CLASS_MAGE:    npcspellid = 16652; break;
                case CLASS_WARLOCK:    npcspellid = 988; break;
                case CLASS_DRUID:    npcspellid = 3036; break;
                }
                player->GetSession()->SendTrainerList(creature->GetGUID(), npcspellid);
                CloseGossipMenuFor(player);
            }
            else if (action == 2) {
                learnPSkill(player, creature);
            }
            else if (action == 3) {
                player->GetSession()->SendTrainerList(creature->GetGUID(), 31238);
                CloseGossipMenuFor(player);
            }
            else if (action == 4) {
                player->GetSession()->SendTrainerList(creature->GetGUID(), 190017);
                CloseGossipMenuFor(player);
            }
            else if (action == 5) {
                CloseGossipMenuFor(player);

                if (player->GetLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL)) {

                    ChatHandler(player->GetSession()).SendNotification("|cffFFFF00系统消息 \n\n |cFFFF0000你现在的等级还不能学习双天赋。");
                }
                else {

                    if (player->GetSpecsCount() == 1) {

                        player->CastSpell(player, 63680, true, NULL, NULL, player->GetGUID());
                        player->CastSpell(player, 63624, true, NULL, NULL, player->GetGUID());
                        ChatHandler(player->GetSession()).SendNotification("|cffFFFF00系统消息 \n\n |cFFFF0000你成功学会的了双天赋!");
                        player->CastSpell(player, 31726);
                        player->PlayerTalkClass->SendCloseGossip();
                    }
                    else {

                        ChatHandler(player->GetSession()).SendNotification("|cffFFFF00系统消息 \n\n |cFFFF0000您已经学会了双天赋，不能重复学习!");
                    }
                }
            }
            else if (action == 6) {
                CloseGossipMenuFor(player);
                // 检查玩家是否学习过天赋  
                if (player->GetLevel() < 10) {
                    player->SendTalentsInfoData(false);
                    ChatHandler(player->GetSession()).SendNotification("|cffFFFF00系统消息 \n\n  |cFFFF0000你还没有学习过天赋!");
                }
                else {
                    int totalTalents = player->GetLevel() >= 10 ? player->GetLevel() - 9 : 0; // 根据等级计算总天赋点数  
                    int currentTalents = player->GetFreeTalentPoints(); // 获取当前剩余天赋点数  

                    if (currentTalents < totalTalents) {
                        // 玩家已经分配了天赋点，并且剩余天赋点数少于总天赋点数，可以重置天赋  
                        player->resetTalents(true);
                        player->SendTalentsInfoData(false);
                        ChatHandler(player->GetSession()).SendNotification("|cffFFFF00系统消息 \n\n  |cFFFF0000你重置了天赋!");
                        player->CastSpell(player, 31726);
                    }
                    else {
                        // 没有分配任何天赋点数，发送通知提示  
                        ChatHandler(player->GetSession()).SendNotification("|cffFFFF00系统消息 \n\n  |cFFFF0000你还没有分配任何天赋点数!");
                    }
                }
            }
            else if (action == 7) {
                player->SetBindPoint(creature->GetGUID());
                CloseGossipMenuFor(player);
            }

            else if (action == 8) {
                CloseGossipMenuFor(player);
                // 检查玩家是否有宠物  
                if (player->GetPet() == nullptr) {
                    // 玩家没有宠物，发送消息  
                    ChatHandler(player->GetSession()).SendNotification("|cffFFFF00系统消息 \n\n  |cFFFF0000你还没有宠物。");
                }
                else {
                    // 玩家有宠物，执行当前逻辑  
                    // 重置玩家的天赋并发送通知    
                    player->ResetPetTalents();
                    player->SendTalentsInfoData(false);
                    ChatHandler(player->GetSession()).SendNotification("|cffFFFF00系统消息 \n\n  |cFFFF0000宠物天赋已重置!");
                    player->CastSpell(player, 31726);
                }
            }
            
        }
        else if (Sender == 2) {
            uint32 npcskillid = 0; //技能训练师ID
            switch (action)
            {
            case 1:     npcskillid = 28698; break; //采矿
            case 2:     npcskillid = 28704; break; //草药
            case 3:     npcskillid = 28703; break; //炼金
            case 4:     npcskillid = 28693; break; //附魔
            case 5:     npcskillid = 28694; break; //锻造
            case 6:     npcskillid = 28699; break; //裁缝
            case 7:     npcskillid = 28697; break; //工程
            case 8:     npcskillid = 28701; break; //珠宝
            case 9:     npcskillid = 28700; break; //制皮
            case 10:    npcskillid = 28696; break; //剥皮
            case 11:    npcskillid = 28702; break; //铭文
            case 12:    npcskillid = 28705; break; //烹饪
            case 13:    npcskillid = 28742; break; //钓鱼
            case 14:    npcskillid = 28706; break; //急救
            }
            player->GetSession()->SendTrainerList(creature->GetGUID(), npcskillid);
            CloseGossipMenuFor(player);
        }
        return true;
    }
    //专业训练菜单
    static void learnPSkill(Player* player, Creature* creature)
    {
        player->PlayerTalkClass->ClearMenus();
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/Trade_Mining:23:23:1:0|t 采矿", 2, 1);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/Trade_Herbalism:23:23:1:0|t 草药", 2, 2);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/Trade_Alchemy:23:23:1:0|t 炼金", 2, 3);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/Trade_Engraving:23:23:1:0|t 附魔", 2, 4);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/trade_blacksmithing:23:23:1:0|t 锻造", 2, 5);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/Trade_Tailoring:23:23:1:0|t 裁缝", 2, 6);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/Icons/Trade_Mining:23:23:1:0|t 工程", 2, 7);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/Icons/INV_Misc_Gem_01:23:23:1:0|t 珠宝", 2, 8);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/Trade_LeatherWorking:23:23:1:0|t 制皮", 2, 9);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/Icons/INV_Misc_LeatherScrap_10:23:23:1:0|t 剥皮", 2, 10);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/Icons/INV_Inscription_Tradeskill01:23:23:1:0|t 铭文", 2, 11);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/Icons/INV_Misc_Food_15:23:23:1:0|t 烹饪", 2, 12);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/Trade_Fishing:23:23:1:0|t 钓鱼", 2, 13);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/Icons/INV_Inscription_Tradeskill01:23:23:1:0|t 急救", 2, 14);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature);
    }
};

void AddSC_SynthesizeTrainer()
{
    new CreatureScript_SynthesizeTrainer();
}
