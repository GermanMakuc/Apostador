#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"
#include "Chat.h"

enum Actions
{
    ACTION_NONE = 0,
    ACTION_DRUGS = 1001,
    ACTION_ONLY_GM_RESULT = 1002,
    ACTION_REWARDS = 1003,
    ACTION_CLOSE = 1004
};

uint32 MoneyConvert(uint32 copper)
{
    uint32 money = copper / 10000;
    return money;
}
uint32 CountDrugs()
{
    QueryResult Conteo = CharacterDatabase.PQuery("SELECT COUNT(*) FROM apuestas");
    Field* fields = Conteo->Fetch();
    uint32 c = fields[0].GetUInt32();
    return c;
}

uint32 getCost()
{
    uint32 Cost = sConfigMgr->GetIntDefault("Cost", 500000);
    return Cost;
}
void RewardMoney(Player* player)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT id, total FROM apuestas_ganadores WHERE guid = %u AND entregado = 0", player->GetSession()->GetGuidLow());
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            uint64 recompensa = fields[1].GetUInt32();
            ChatHandler(player->GetSession()).PSendSysMessage("Has obtenido una recompensa de [%u oro] por una apuesta ganada.", MoneyConvert(recompensa));
            player->ModifyMoney(recompensa);
            CharacterDatabase.PQuery("UPDATE apuestas_ganadores SET entregado = 1 WHERE id = %u", id);
        } while (result->NextRow());

    }
}

class Apostador : public CreatureScript
{
public:

    Apostador() : CreatureScript("Apostador") { }
    
	bool OnGossipHello(Player* player, Creature* creature)
	{
        uint32 COST = getCost();
        uint32 guid = player->GetSession()->GetGuidLow();
		QueryResult result = CharacterDatabase.PQuery("SELECT 1 FROM apuestas WHERE guid = %u LIMIT 1", guid);
        QueryResult winner = CharacterDatabase.PQuery("SELECT 1 FROM apuestas_ganadores WHERE guid = %u AND entregado = 0 LIMIT 1", guid);
        if (result)
		{
			std::stringstream ss;
            ss << "Ya hiciste la apuesta de  " << (MoneyConvert(COST)) << " de oro debes esperar que finalice, existen " << CountDrugs() << " postores en este momento activos.";
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, ss.str().c_str(), GOSSIP_SENDER_MAIN, ACTION_CLOSE);
		}
		else
		{
			std::stringstream st;
			st << "¿Deseas apostar una monedita?, parte en " << (MoneyConvert(COST)) << " de oro y existen " << CountDrugs() << " postores en este momento activos.";
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, st.str().c_str(), GOSSIP_SENDER_MAIN, ACTION_DRUGS);
		}
        if (player->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
        {
            if (CountDrugs() > 0)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Dar por término la apuesta y escoger al ganador (Opción sólo visible para GM)", GOSSIP_SENDER_MAIN, ACTION_ONLY_GM_RESULT);
        }
        if (winner)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Tienes una o más apuestas ganadas sin cobrar, ¿deseas recibirlas?", GOSSIP_SENDER_MAIN, ACTION_REWARDS);

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Salir", GOSSIP_SENDER_MAIN, ACTION_CLOSE);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}
    
	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
		player->PlayerTalkClass->ClearMenus();
        uint32 COST = getCost();
		switch(action)
		{
			case ACTION_CLOSE:
				player->CLOSE_GOSSIP_MENU();
				return true;
			case ACTION_NONE:
				return OnGossipHello(player, creature);
			case ACTION_DRUGS:
				if (!player->HasEnoughMoney(COST))
                    player->GetSession()->SendNotification("No tienes %u oro para la apuesta miníma que necesitas para apostar.", MoneyConvert(COST));
				else
				{
					player->ModifyMoney(-COST);
					CharacterDatabase.PExecute("INSERT INTO apuestas(guid, nombre, apuesta) VALUES (%u,'%s', %u)", player->GetSession()->GetGuidLow(), player->GetSession()->GetPlayerName(), COST);
				}
			break;
            case ACTION_REWARDS:
                RewardMoney(player);
            break;
            case ACTION_ONLY_GM_RESULT:
                QueryResult qr = CharacterDatabase.PQuery("SELECT guid, nombre FROM apuestas ORDER BY RAND() LIMIT 1");
                uint32 GUID = qr->Fetch()[0].GetUInt32();
                std::string name = qr->Fetch()[1].GetCString();
                std::string tag_colour = "7bbef7";
                std::string player_colour = "7bbef7";
                std::string winner_colour = "00ff00";
                std::string money_colour = "ff0000";
                std::ostringstream stream;
                    
                stream << "|CFF" << tag_colour << "|r|cff" << player_colour << " " << name << " Es el ganador de la apuesta |cff" << winner_colour << "|r Ha ganado |CFF" << money_colour << "[" << MoneyConvert(COST * CountDrugs()) <<" Oro] entre " << CountDrugs() << " postores.";
                sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());

                CharacterDatabase.PExecute("INSERT INTO apuestas_ganadores(guid, nombre, total, entregado) VALUES (%u, '%s', %u, %u)", GUID, name,(COST * CountDrugs()), 0);
                CharacterDatabase.PExecute("TRUNCATE TABLE apuestas");
            break;
		}
		player->CLOSE_GOSSIP_MENU();
        return true;
	}
    
};

class Recompensas : public PlayerScript
{
public:
    Recompensas() : PlayerScript("rewards") {}

    void OnLogin(Player* player) override
    {
        RewardMoney(player);
    }
};

class MyWorldScript : public WorldScript
{
public:

    MyWorldScript() : WorldScript("MyWorldScript") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/Apostador.conf";
            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
};

void AddApostadorScripts() {
    new Apostador();
    new Recompensas();
    new MyWorldScript();
}

