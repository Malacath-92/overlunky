#include "state_lua.hpp"

#include "level_api.hpp"
#include "state.hpp"

#include <sol/sol.hpp>

namespace NState
{
void register_usertypes(sol::state& lua)
{
    lua.new_usertype<SelectPlayerSlot>(
        "SelectPlayerSlot",
        "activated",
        &SelectPlayerSlot::activated,
        "character",
        &SelectPlayerSlot::character,
        "texture",
        &SelectPlayerSlot::texture_id);
    lua.new_usertype<Items>(
        "Items",
        "player_select",
        sol::property([](Items& s)
                      { return std::ref(s.player_select_slots); }),
        "player_count",
        &Items::player_count);
    lua.new_usertype<StateMemory>(
        "StateMemory",
        "screen_last",
        &StateMemory::screen_last,
        "screen",
        &StateMemory::screen,
        "screen_next",
        &StateMemory::screen_next,
        "ingame",
        &StateMemory::ingame,
        "playing",
        &StateMemory::playing,
        "pause",
        &StateMemory::pause,
        "width",
        &StateMemory::w,
        "height",
        &StateMemory::h,
        "kali_favor",
        &StateMemory::kali_favor,
        "kali_status",
        &StateMemory::kali_status,
        "kali_altars_destroyed",
        &StateMemory::kali_altars_destroyed,
        "seed",
        &StateMemory::seed,
        "time_total",
        &StateMemory::time_total,
        "world",
        &StateMemory::world,
        "world_next",
        &StateMemory::world_next,
        "world_start",
        &StateMemory::world_start,
        "level",
        &StateMemory::level,
        "level_next",
        &StateMemory::level_next,
        "level_start",
        &StateMemory::level_start,
        "theme",
        &StateMemory::theme,
        "theme_next",
        &StateMemory::theme_next,
        "theme_start",
        &StateMemory::theme_start,
        "shoppie_aggro",
        &StateMemory::shoppie_aggro,
        "shoppie_aggro_next",
        &StateMemory::shoppie_aggro_levels,
        "merchant_aggro",
        &StateMemory::merchant_aggro,
        "kills_npc",
        &StateMemory::kills_npc,
        "level_count",
        &StateMemory::level_count,
        "journal_flags",
        &StateMemory::journal_flags,
        "time_last_level",
        &StateMemory::time_last_level,
        "time_level",
        &StateMemory::time_level,
        "level_flags",
        &StateMemory::hud_flags,
        "loading",
        &StateMemory::loading,
        "quest_flags",
        &StateMemory::quest_flags,
        "presence_flags",
        &StateMemory::presence_flags,
        "fadevalue",
        &StateMemory::fadevalue,
        "fadeout",
        &StateMemory::fadeout,
        "fadein",
        &StateMemory::fadein,
        "loading_black_screen_timer",
        &StateMemory::loading_black_screen_timer,
        "saved_dogs",
        &StateMemory::saved_dogs,
        "saved_cats",
        &StateMemory::saved_cats,
        "saved_hamsters",
        &StateMemory::saved_hamsters,
        "win_state",
        &StateMemory::win_state,
        "illumination",
        &StateMemory::illumination,
        "money_last_levels",
        &StateMemory::money_last_levels,
        "money_shop_total",
        &StateMemory::money_shop_total,
        "player_inputs",
        sol::readonly(&StateMemory::player_inputs),
        "quests",
        &StateMemory::quests,
        "camera",
        &StateMemory::camera,
        "special_visibility_flags",
        &StateMemory::special_visibility_flags,
        "cause_of_death",
        &StateMemory::cause_of_death,
        "cause_of_death_entity_type",
        &StateMemory::cause_of_death_entity_type,
        "toast_timer",
        &StateMemory::toast_timer,
        "speechbubble_timer",
        &StateMemory::speechbubble_timer,
        "speechbubble_owner",
        &StateMemory::speechbubble_owner,
        "level_gen",
        &StateMemory::level_gen,
        "correct_ushabti",
        &StateMemory::correct_ushabti,
        "items",
        &StateMemory::items,
        "camera_layer",
        &StateMemory::camera_layer,
        "get_correct_ushabti",
        &StateMemory::get_correct_ushabti,
        "set_correct_ushabti",
        &StateMemory::set_correct_ushabti);
    lua.new_usertype<LightParams>(
        "LightParams",
        "red",
        &LightParams::red,
        "green",
        &LightParams::green,
        "blue",
        &LightParams::blue,
        "size",
        &LightParams::size);
    lua.new_usertype<Illumination>(
        "Illumination",
        "lights",
        &Illumination::lights,
        "light1",
        &Illumination::light1,
        "light2",
        &Illumination::light2,
        "light3",
        &Illumination::light3,
        "light4",
        &Illumination::light4,
        "brightness",
        &Illumination::brightness,
        "brightness_multiplier",
        &Illumination::brightness_multiplier,
        "light_pos_x",
        &Illumination::light_pos_x,
        "light_pos_y",
        &Illumination::light_pos_y,
        "offset_x",
        &Illumination::offset_x,
        "offset_y",
        &Illumination::offset_y,
        "distortion",
        &Illumination::distortion,
        "entity_uid",
        &Illumination::entity_uid,
        "flags",
        &Illumination::flags);
    lua.new_usertype<Camera>(
        "Camera",
        "bounds_left",
        &Camera::bounds_left,
        "bounds_right",
        &Camera::bounds_right,
        "bounds_bottom",
        &Camera::bounds_bottom,
        "bounds_top",
        &Camera::bounds_top,
        "adjusted_focus_x",
        &Camera::adjusted_focus_x,
        "adjusted_focus_y",
        &Camera::adjusted_focus_y,
        "focus_offset_x",
        &Camera::focus_offset_x,
        "focus_offset_y",
        &Camera::focus_offset_y,
        "focus_x",
        &Camera::focus_x,
        "focus_y",
        &Camera::focus_y,
        "vertical_pan",
        &Camera::vertical_pan,
        "shake_countdown_start",
        &Camera::shake_countdown_start,
        "shake_countdown",
        &Camera::shake_countdown,
        "shake_amplitude",
        &Camera::shake_amplitude,
        "shake_multiplier_x",
        &Camera::shake_multiplier_x,
        "shake_multiplier_y",
        &Camera::shake_multiplier_y,
        "uniform_shake",
        &Camera::uniform_shake,
        "focused_entity_uid",
        &Camera::focused_entity_uid,
        "inertia",
        &Camera::inertia);

    lua.create_named_table("CAUSE_OF_DEATH", "DEATH", 0, "ENTITY", 1, "LONG_FALL", 2, "STILL_FALLING", 3, "MISSED", 4, "POISONED", 5);

    lua["toast_visible"] = []() -> bool
    {
        return State::get().ptr()->toast != 0;
    };

    lua["speechbubble_visible"] = []() -> bool
    {
        return State::get().ptr()->speechbubble != 0;
    };

    lua["cancel_toast"] = []()
    {
        State::get().ptr()->toast_timer = 1000;
    };

    lua["cancel_speechbubble"] = []()
    {
        State::get().ptr()->speechbubble_timer = 1000;
    };
}
}; // namespace NState
