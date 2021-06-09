#include "level_lua.hpp"

#include "level_api.hpp"
#include "savedata.hpp"
#include "script/script_impl.hpp"
#include "state.hpp"

#include <sol/sol.hpp>

namespace NLevel
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
    /// Add a callback for a specific tile code that is called before the game handles the tile code.
    /// Return true in order to stop the game or scripts loaded after this script from handling this tile code.
    /// For example, when returning true in this callback set for `"floor"` then no floor will spawn in the game (unless you spawn it yourself)
    lua["set_pre_tile_code_callback"] = [script](sol::function cb, std::string tile_code) -> CallbackId
    {
        script->pre_level_gen_callbacks.push_back(LevelGenCallback{script->cbcount, std::move(tile_code), std::move(cb)});
        return script->cbcount++;
    };
    /// Add a callback for a specific tile code that is called after the game handles the tile code.
    /// Use this to affect what the game or other scripts spawned in this position.
    /// This is received even if a previous pre-tile-code-callback has returned true
    lua["set_post_tile_code_callback"] = [script](sol::function cb, std::string tile_code) -> CallbackId
    {
        script->post_level_gen_callbacks.push_back(LevelGenCallback{script->cbcount, std::move(tile_code), std::move(cb)});
        return script->cbcount++;
    };
    /// Define a new tile code, to make this tile code do anything you have to use either `set_pre_tile_code_callback` or `set_post_tile_code_callback`.
    /// If a user disables your script but still uses your level mod nothing will be spawned in place of your tile code.
    lua["define_tile_code"] = [script](std::string tile_code)
    {
        script->g_state->level_gen->data->define_tile_code(std::move(tile_code));
    };

    lua.new_usertype<QuestsInfo>(
        "QuestsInfo",
        "yang_state",
        &QuestsInfo::yang_state,
        "jungle_sisters_flags",
        &QuestsInfo::jungle_sisters_flags,
        "van_horsing_state",
        &QuestsInfo::van_horsing_state,
        "sparrow_state",
        &QuestsInfo::sparrow_state,
        "madame_tusk_state",
        &QuestsInfo::madame_tusk_state,
        "beg_state",
        &QuestsInfo::beg_state);

#define table_of(T, name) \
    sol::property([script]() { return sol::as_table_ref(std::vector<T>(script->g_save->name, script->g_save->name + sizeof script->g_save->name / sizeof script->g_save->name[0])); })

    lua.new_usertype<SaveData>(
        "SaveData",
        "places",
        table_of(bool, places),
        "bestiary",
        table_of(bool, bestiary),
        "people",
        table_of(bool, people),
        "items",
        table_of(bool, items),
        "traps",
        table_of(bool, traps),
        "last_daily",
        sol::readonly(&SaveData::last_daily),
        "characters",
        sol::readonly(&SaveData::characters),
        "shortcuts",
        sol::readonly(&SaveData::shortcuts),
        "bestiary_killed",
        table_of(int, bestiary_killed),
        "bestiary_killed_by",
        table_of(int, bestiary_killed_by),
        "people_killed",
        table_of(int, people_killed),
        "people_killed_by",
        table_of(int, people_killed_by),
        "plays",
        sol::readonly(&SaveData::plays),
        "deaths",
        sol::readonly(&SaveData::deaths),
        "wins_normal",
        sol::readonly(&SaveData::wins_normal),
        "wins_hard",
        sol::readonly(&SaveData::wins_hard),
        "wins_special",
        sol::readonly(&SaveData::wins_special),
        "score_total",
        sol::readonly(&SaveData::score_total),
        "score_top",
        sol::readonly(&SaveData::score_top),
        "deepest_area",
        sol::readonly(&SaveData::deepest_area),
        "deepest_level",
        sol::readonly(&SaveData::deepest_level));

    lua.create_named_table("LAYER", "FRONT", 0, "BACK", 1, "PLAYER", -1, "PLAYER1", -1, "PLAYER2", -2, "PLAYER3", -3, "PLAYER4", -4);
    lua.create_named_table(
        "THEME",
        "DWELLING",
        1,
        "JUNGLE",
        2,
        "VOLCANA",
        3,
        "OLMEC",
        4,
        "TIDE_POOL",
        5,
        "TEMPLE",
        6,
        "ICE_CAVES",
        7,
        "NEO_BABYLON",
        8,
        "SUNKEN_CITY",
        9,
        "COSMIC_OCEAN",
        10,
        "CITY_OF_GOLD",
        11,
        "DUAT",
        12,
        "ABZU",
        13,
        "TIAMAT",
        14,
        "EGGPLANT_WORLD",
        15,
        "HUNDUN",
        16,
        "BASE_CAMP",
        17,
        "ARENA",
        18);

    /// Parameter to force_co_subtheme
    lua.create_named_table("COSUBTHEME", "RESET", -1, "DWELLING", 0, "JUNGLE", 1, "VOLCANA", 2, "TIDEPOOL", 3, "TEMPLE", 4, "ICECAVES", 5, "NEOBABYLON", 6, "SUNKENCITY", 7);

    /// Yang quest states
    lua.create_named_table("YANG", "ANGRY", -1, "QUEST_NOT_STARTED", 0, "TURKEY_PEN_SPAWNED", 2, "BOTH_TURKEYS_DELIVERED", 3, "TURKEY_SHOP_SPAWNED", 4, "ONE_TURKEY_BOUGHT", 5, "TWO_TURKEYS_BOUGHT", 6, "THREE_TURKEYS_BOUGHT", 7);

    /// Jungle sister quest flags (angry = -1)
    lua.create_named_table("JUNGLESISTERS", "PARSLEY_RESCUED", 1, "PARSNIP_RESCUED", 2, "PARMESAN_RESCUED", 3, "WARNING_ONE_WAY_DOOR", 4, "GREAT_PARTY_HUH", 5, "I_WISH_BROUGHT_A_JACKET", 6);

    /// Van Horsing quest states
    lua.create_named_table("VANHORSING", "QUEST_NOT_STARTED", 0, "JAILCELL_SPAWNED", 1, "FIRST_ENCOUNTER_DIAMOND_THROWN", 2, "SPAWNED_IN_VLADS_CASTLE", 3, "SHOT_VLAD", 4, "TEMPLE_HIDEOUT_SPAWNED", 5, "SECOND_ENCOUNTER_COMPASS_THROWN", 6, "TUSK_CELLAR", 7);

    /// Sparrow quest states
    lua.create_named_table("SPARROW", "QUEST_NOT_STARTED", 0, "THIEF_STATUS", 1, "FINISHED_LEVEL_WITH_THIEF_STATUS", 2, "FIRST_HIDEOUT_SPAWNED_ROPE_THROW", 3, "FIRST_ENCOUNTER_ROPES_THROWN", 4, "TUSK_IDOL_STOLEN", 5, "SECOND_HIDEOUT_SPAWNED_NEOBAB", 6, "SECOND_ENCOUNTER_INTERACTED", 7, "MEETING_AT_TUSK_BASEMENT", 8);

    /// Madame Tusk quest states
    lua.create_named_table("TUSK", "ANGRY", -2, "DEAD", -1, "QUEST_NOT_STARTED", 0, "DICE_HOUSE_SPAWNED", 1, "HIGH_ROLLER_STATUS", 2, "PALACE_WELCOME_MESSAGE", 3);

    /// Beg quest states
    lua.create_named_table("BEG", "QUEST_NOT_STARTED", 0, "ALTAR_DESTROYED", 1, "SPAWNED_WITH_BOMBBAG", 2, "BOMBBAG_THROWN", 3, "SPAWNED_WITH_TRUECROWN", 4, "TRUECROWN_THROWN", 5);
}
}; // namespace NLevel
