mod critical_section;
mod db;
mod memory;
mod models;
mod search;
mod ui;

mod test;

use backtrace::Backtrace;
use critical_section::CriticalSectionManager;
use db::list_entities;
use log::log_enabled;
use memory::Memory;
use models::{API, State, GameState};
use std::thread;
use std::time;

use winapi::um::{
    consoleapi::SetConsoleCtrlHandler,
    processthreadsapi::ExitThread,
    wincon::AttachConsole,
    wincon::FreeConsole,
    wincon::{CTRL_BREAK_EVENT, CTRL_CLOSE_EVENT, CTRL_C_EVENT},
};

use rhai::{Scope, Engine, Dynamic, EvalAltResult, RegisterFn};

#[no_mangle]
pub extern "C" fn DllMain(_: *const u8, _reason: u32, _: *const u8) -> u32 {
    1 // TRUE
}

unsafe extern "system" fn ctrl_handler(ctrl_type: u32) -> i32 {
    match ctrl_type {
        CTRL_C_EVENT | CTRL_BREAK_EVENT | CTRL_CLOSE_EVENT => {
            log::debug!("Console detached, you can now close this window.");
            FreeConsole();
            1
        }
        _ => 0,
    }
}

unsafe fn attach_stdout(pid: u32) {
    let env = env_logger::Env::default().filter_or(env_logger::DEFAULT_FILTER_ENV, "info");
    env_logger::Builder::from_env(env).init();
    if log_enabled!(log::Level::Debug) {
        AttachConsole(pid);
        SetConsoleCtrlHandler(Some(ctrl_handler), 1);
    }
}

unsafe fn set_panic_hook() {
    std::panic::set_hook(Box::new(|panic_info| {
        if let Some(s) = panic_info.payload().downcast_ref::<&str>() {
            log::error!("panic: {}", s);
        }
        log::error!("{:?}", Backtrace::new());
        FreeConsole();
        ExitThread(0);
    }));
}

#[derive(Copy, Clone)]
pub struct Game {
    current: GameState,
    old: GameState,
    state: State,
}

impl Game {
    pub fn new(state: State) -> Game {
        Game {
            current: state.gamestate(),
            old: state.gamestate(),
            state: state
        }
    }
    pub fn update(&mut self) {
        self.old = self.current;
        self.current = self.state.gamestate();
    }
    pub fn spawn_entity(&mut self, item: i64, dx: i64, dy: i64) {
        match self.state.items().player(0) {
            None => {
                log::debug!("Script tried to spawn but no player");
            }
            Some(player) => {
                log::debug!("Script spawning a thing");
                let (x, y) = player.position();
                let layer = player.layer();
                unsafe {
                    self.state.layer(layer).spawn_entity(item as usize, x + dx as f32, y + dy as f32, false);
                }
            }
        }
    }
    pub fn current(&mut self) -> GameState {
        self.current
    }
    pub fn old(&mut self) -> GameState {
        self.old
    }
}

#[no_mangle]
unsafe extern "C" fn run(handle: u32) {
    attach_stdout(handle);
    set_panic_hook();
    log::debug!("Game injected! Press Ctrl+C to detach this window from the process.");

    let memory = Memory::get();
    let state = State::new();
    loop {
        let entities = list_entities(&memory);
        if entities.len() != 0 {
            ui::create_box(&entities);
            break;
        }
        thread::sleep(time::Duration::from_millis(100));
    }

    let api = API::new(&memory);

    if let Err(err) = ui::ffi::init_hooks(api.swap_chain()) {
        log::error!("{}", err);
        return;
    }

    let mut game = Game::new(state);
    let mut engine = Engine::new();
    let mut scope = Scope::new();
    engine.register_type::<GameState>();
    engine.register_type::<Game>();
    engine.register_fn("game", move || game);
    engine.register_fn("spawn_entity", Game::spawn_entity);
    let mut level_starting: bool = false;
    match engine.compile_file("Overlunky/script.rhai".into()) {
        Err(e) => {
            log::error!("Compile error: {:?}", e);
        }
        Ok(ast) => {
            loop {
                match state.items().player(0) {
                    None => {
                        //log::debug!("Can't find player");
                    }
                    Some(player) => {
                        game.update();
                        //log::debug!("{:?}", &game.current());
                        /*if game.current().igt() > game.old().igt() {
                            match engine.call_fn_dynamic(&mut scope, &ast, "frame", None, [game.current().world().into(), game.current().level().into()]) {
                                Err(e) => {
                                    log::error!("Script error: {:?}", e);
                                }
                                Ok(result) => {
                                    log::debug!("Result: {:?}", result);
                                }
                            }
                        }*/
                        if (game.current().igt() < game.old().igt() && game.current().screen() == 12) || (game.current().screen() == 12 && (game.old().screen() == 11 || game.old().screen() == 13)) {
                            log::debug!("Level starting, waiting for it to load...");
                            level_starting = true;
                        }
                        if level_starting && game.current().playing() == 1 && game.current().ingame() == 1 && game.current().pause() == 0 && game.current().igt() > game.old().igt() {
                            level_starting = false;
                            log::debug!("Running level script");
                            match engine.call_fn_dynamic(&mut scope, &ast, "level", None, [game.current().world().into(), game.current().level().into()]) {
                                Err(e) => {
                                    log::error!("Script error: {:?}", e);
                                }
                                Ok(result) => {
                                    log::debug!("Result: {:?}", result);
                                }
                            }
                        }
                    }
                }
                thread::sleep(time::Duration::from_millis(16));
            }
        }
    }

    /*if log_enabled!(log::Level::Debug) {
        let c = CriticalSectionManager::new();
        loop {
            log::debug!("Enter entity #IDs to spawn, one per line >");
            let mut buffer = String::new();
            std::io::stdin().read_line(&mut buffer).unwrap();
            if let Ok(item) = buffer.trim().parse::<usize>() {
                // This is RAII-style implementation for suspending the main thread, for preventing race conditions.
                let mut _lock = c.lock();

                {
                    // This is RAII-style implementation for suspending the main thread, for preventing race conditions.
                    let mut _lock = c.lock();

                    match state.items().player(0) {
                        None => {
                            log::error!("Player not initialized yet. Select a character first!");
                        }
                        Some(player) => {
                            let (x, y) = player.position();
                            let layer = player.layer();
                            state.layer(layer).spawn_entity(item, x, y, false);
                        }
                    }
                }
            }
        }
    }*/
}
