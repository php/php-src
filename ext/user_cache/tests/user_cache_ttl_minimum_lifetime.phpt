--TEST--
UserCache\Cache: a TTL lasts at least its length on the monotonic clock and ends shortly after
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
--FILE--
<?php
require __DIR__ . '/user_cache_clock.inc';

$cache = UserCache\Cache::getPool('ttl-minimum');
$cache->clear();

$start = hrtime();
var_dump($cache->store('short', 'value', 1));
$hits = 0;
$misses = 0;
while (seconds_since($start) < 0.9) {
    if ($cache->fetch('short') === 'value') {
        $hits++;
    } else {
        $misses++;
    }
    usleep(10000);
}
var_dump($hits > 0, $misses);

usleep(max(0, (int) ((1.2 - seconds_since($start)) * 1000000)));
var_dump($cache->fetch('short', 'expired'));
?>
--EXPECT--
bool(true)
bool(true)
int(0)
string(7) "expired"
