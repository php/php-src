--TEST--
UserCache\Cache: pool factory returns per-name singletons
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
var_dump(UserCache\Cache::hasPool('factory'));

$a = UserCache\Cache::getPool('factory');
$b = UserCache\Cache::getPool('factory');
$c = UserCache\Cache::getPool('factory-other');

var_dump($a === $b);
var_dump($a === $c);
var_dump(UserCache\Cache::hasPool('factory'));
var_dump(UserCache\Cache::hasPool('factory-unknown'));

$pools = UserCache\Cache::getPools();
ksort($pools);
var_dump(array_keys($pools));
var_dump($pools['factory'] === $a);

try {
    new UserCache\Cache();
} catch (Error $e) {
    echo get_class($e), "\n";
}

try {
    UserCache\Cache::getPool('');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    UserCache\Cache::getPool("bad\x1fpool");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($a->store('shared-key', 'from-factory'));
var_dump($c->fetch('shared-key', 'MISS'));
var_dump($a->fetch('shared-key'));
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
array(2) {
  [0]=>
  string(7) "factory"
  [1]=>
  string(13) "factory-other"
}
bool(true)
Error
UserCache\Cache::getPool(): Argument #1 ($pool) must not be empty
UserCache\Cache::getPool(): Argument #1 ($pool) must not contain the user-cache key delimiter 0x1F
bool(true)
string(4) "MISS"
string(12) "from-factory"
