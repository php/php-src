--TEST--
UserCache\Cache: API calls from a session save handler after rshutdown are gated
--EXTENSIONS--
session
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
session.save_path=
session.use_cookies=0
session.use_trans_sid=0
session.cache_limiter=
--FILE--
<?php
session_set_save_handler(new class implements SessionHandlerInterface {
    public function open(string $path, string $name): bool { return true; }
    public function close(): bool { return true; }
    public function read(string $id): string|false { return ''; }
    public function destroy(string $id): bool { return true; }
    public function gc(int $max_lifetime): int|false { return 0; }

    public function write(string $id, string $data): bool {
        echo "in shutdown window\n";
        $pool = UserCache\Cache::getPool('during-shutdown');
        var_dump($pool->store('k', 'v'));
        var_dump($pool->fetch('k', 'default'));
        var_dump($pool->lock('k'));
        var_dump(UserCache\Cache::getStatus()->getAvailability()->name);

        return true;
    }
});

class LateSession {
    public function __destruct() {
        session_start();
        $_SESSION['x'] = 1;
    }
}
$GLOBALS['late'] = new LateSession();

$cache = UserCache\Cache::getPool('main');
$cache->clear();
var_dump($cache->store('before', 1));
var_dump(UserCache\Cache::getStatus()->getAvailability()->name);
echo "script end\n";
?>
--EXPECT--
bool(true)
string(9) "Available"
script end
in shutdown window
bool(false)
string(7) "default"
bool(false)
string(26) "UnavailableByUnknownReason"
