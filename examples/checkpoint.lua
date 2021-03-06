meta.name = 'Ankh Checkpoint'
meta.version = 'WIP'
meta.description = 'If you die with an ankh, you get revived where you picked it up and get a new ankh. Only works in the front layer!'
meta.author = 'Dregu'

cp = {0, 0, 0}
ankhs = {}
revive = -1
died = 0

function dist(ax, ay, bx, by)
    return math.sqrt((bx - ax)^2 + (by - ay)^2)
end

set_callback(function()
    set_timeout(function()
        set_interval(function()
            for i, v in ipairs(get_entities_by_type(ENT_TYPE.ITEM_PICKUP_ANKH)) do
                ent = get_entity(v)
                ent.flags = set_flag(ent.flags, 10)
                x, y, l = get_position(v)
                ankhs[v] = {x, y + 0.1, l}
            end

            for i, v in pairs(ankhs) do
                ent = get_entity(i)
                if ent == nil or ent.type.id ~= ENT_TYPE.ITEM_PICKUP_ANKH then
                    px, py, pl = get_position(players[1].uid)
                    if dist(px, py, v[1], v[2]) < 1 and get_frame() > died+60 then
                        cp = v
                        toast('Checkpoint!')
                    end
                    ankhs[i] = nil
                end
            end

            if entity_has_item_type(players[1].uid, ENT_TYPE.ITEM_POWERUP_ANKH) then
                if players[1].health == 0 then
                    died = get_frame()
                    set_timeout(function()
                        move_entity(players[1].uid, cp[1], cp[2], LAYER.FRONT, 0, 0)
                    end, 3)
                    if revive == -1 then
                        revive = set_timeout(function()
                            spawn_entity_over(ENT_TYPE.ITEM_PICKUP_ANKH, players[1].uid, 0, 0)
                            revive = -1
                        end, 4)
                    end
                end
            end
        end, 1)
    end, 15)
    x, y, l = get_position(players[1].uid)
    cp = {x, y, l}
    ankhs = {}
    died = 0
end, ON.LEVEL)
