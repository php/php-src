--TEST--
UserCache\Cache: per-key records cache fetched values without serving stale or shared state
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
class OverwriteOnWakeup
{
    public string $target = '';

    public function __wakeup(): void
    {
        UserCache\Cache::getPool('records')->store($this->target, str_repeat('n', 300));
    }
}

$cache = UserCache\Cache::getPool('records');
$array = ['list' => range(1, 8), 'text' => str_repeat('t', 300)];
$long = str_repeat('L', 5000);

/* Cached values stay private to each fetch. */
$cache->store('array', $array);
$cache->store('string', str_repeat('s', 300));
$cache->store('long', $long);
for ($i = 0; $i < 3; $i++) {
    $value = $cache->fetch('array');
    $value['list'][0] = -1;
    $value['text'][0] = 'x';
    $string = $cache->fetch('string');
    $string[0] = 'x';
    $big = $cache->fetch('long');
    $big[0] = 'x';
}
var_dump($cache->fetch('array') === $array, $cache->fetch('string') === str_repeat('s', 300), $cache->fetch('long') === $long);

/* Writes to other keys keep cached values valid; writes to the key replace them. */
$cache->store('other', 1);
var_dump($cache->fetch('array') === $array);
$cache->store('array', ['replaced']);
var_dump($cache->fetch('array'));

/* Deletion, re-addition and misses. */
$cache->fetch('gone', 'unused');
$cache->store('gone', 'here');
var_dump($cache->fetch('gone'), $cache->delete('gone'), $cache->fetch('gone', 'default'), $cache->has('gone'));
$cache->store('gone', 'again');
var_dump($cache->fetch('gone'), $cache->has('gone'));

/* Scalar overwrites and counters update the record they read through. */
$cache->store('counter', 1);
$cache->store('counter', 2);
var_dump($cache->fetch('counter'), $cache->increment('counter', 5), $cache->fetch('counter'));
$cache->store('counter', 'text');
var_dump($cache->fetch('counter'));

/* More keys than a pool keeps inline, including numeric strings. */
$values = [];
for ($i = 0; $i < 20; $i++) {
    $values[(string) $i] = [$i];
}
var_dump($cache->storeMultiple($values));
$fetched = $cache->fetchMultiple(array_keys($values));
var_dump($fetched === $values, $cache->fetch('7') === [7], $cache->fetch('19') === [19]);

/* A __wakeup() running inside fetchMultiple() rewrites a key fetched earlier. */
$cache->store('early', str_repeat('o', 300));
$trigger = new OverwriteOnWakeup();
$trigger->target = 'early';
$cache->store('trigger', $trigger);
$both = $cache->fetchMultiple(['early', 'trigger']);
var_dump($both['early'] === str_repeat('o', 300), $cache->fetch('early') === str_repeat('n', 300));

/* Cached values follow expiry and type changes of their entry. */
$cache->store('ttl', $array, 1);
$cache->fetch('ttl');
$cache->fetch('ttl');
usleep(1100000);
var_dump($cache->fetch('ttl', 'expired'));
$cache->store('ttl', $array);
$cache->fetch('ttl');
$cache->store('ttl', 123);
var_dump($cache->fetch('ttl'));

/* clear() and deletePool() drop the pool's cached values. */
var_dump($cache->clear(), $cache->fetch('string', 'cleared'), $cache->fetch('counter', 'cleared'));
$cache->store('string', 'restored');
var_dump($cache->fetch('string'));
var_dump(UserCache\Cache::deletePool('records'), $cache->fetch('string', 'deleted'));
$fresh = UserCache\Cache::getPool('records');
$fresh->store('string', 'fresh');
var_dump($fresh->fetch('string'), $cache->fetch('string'));

/* Records made before fork() revalidate in both processes. */
$fresh->store('shared', ['parent']);
$fresh->fetch('shared');
$pid = pcntl_fork();
if ($pid === 0) {
    $ok = $fresh->fetch('shared') === ['parent'];
    $fresh->store('shared', ['child']);
    exit($ok && $fresh->fetch('shared') === ['child'] ? 0 : 1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status), $fresh->fetch('shared'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [0]=>
  string(8) "replaced"
}
string(4) "here"
bool(true)
string(7) "default"
bool(false)
string(5) "again"
bool(true)
int(2)
int(7)
int(7)
string(4) "text"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "expired"
int(123)
bool(true)
string(7) "cleared"
string(7) "cleared"
string(8) "restored"
bool(true)
string(7) "deleted"
string(5) "fresh"
string(5) "fresh"
int(0)
array(1) {
  [0]=>
  string(5) "child"
}
