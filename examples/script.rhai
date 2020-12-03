// Stick me in Spelunky 2/Overlunky/script.rhai to load and run at each level

fn level(w, l) {
    if w == 1 && l == 1 {
        for n in range(0, 10) {
            game().spawn_entity(536, 0, 0);
            game().spawn_entity(512, 0, 0);
        }
        for n in range(520, 524) {
            game().spawn_entity(n, 0, 0);
        }
        for n in range(526, 528) {
            game().spawn_entity(n, 0, 0);
        }
        for n in range(532, 534) {
            game().spawn_entity(n, 0, 0);
        }
        game().spawn_entity(560, -1, 0);
        game().spawn_entity(558, 0, 0);
        game().spawn_entity(570, 1, 0);
    }
    game().spawn_entity(312, 0, 3);
}
