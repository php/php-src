--TEST--
UserCache\Cache: fetch hit and miss counters per pool and per segment
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) {
	die('skip requires pcntl');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$a = UserCache\Cache::getPool('hits-a');
$b = UserCache\Cache::getPool('hits-b');

function counts(): string {
    $status = UserCache\Cache::getStatus();
    $a = UserCache\Cache::getPool('hits-a')->getPoolStatus();
    $b = UserCache\Cache::getPool('hits-b')->getPoolStatus();

    return sprintf(
        'total %d/%d a %d/%d b %d/%d',
        $status->getHitCount(), $status->getMissCount(),
        $a->getHitCount(), $a->getMissCount(),
        $b->getHitCount(), $b->getMissCount()
    );
}

echo counts(), "\n";

var_dump($a->fetch('missing', 'default'));
echo counts(), "\n";

var_dump($a->store('k', 'v'));
var_dump($a->fetch('k'));
var_dump($a->fetch('k'));
echo counts(), "\n";

/* has() is not a lookup. */
var_dump($a->has('k'), $a->has('missing'));
echo counts(), "\n";

var_dump($a->store('m', ['x' => 1]));
var_dump($a->fetchMultiple(['k', 'm', 'nope']));
echo counts(), "\n";

/* remember() counts its first lookup only. */
var_dump($b->remember('r', static fn () => 'computed'));
var_dump($b->remember('r', static fn () => 'never'));
echo counts(), "\n";

/* An expired entry is a miss. */
var_dump($b->store('ttl', 1, 1));
sleep(2);
var_dump($b->fetch('ttl', 'expired'));
echo counts(), "\n";

/* Counts folded in by another process become visible. */
$pid = pcntl_fork();
if ($pid === 0) {
    $child = UserCache\Cache::getPool('hits-a');
    $child->fetch('k');
    $child->fetch('k');
    $child->fetch('missing');
    exit(0);
}
pcntl_waitpid($pid, $status);
echo counts(), "\n";

/* deletePool() drops the pool's counters with its entries; the segment
 * totals are lifetime counters. */
var_dump(UserCache\Cache::deletePool('hits-b'));
echo counts(), "\n";

var_dump(UserCache\Cache::getPool('hits-b')->fetch('r', 'gone'));
echo counts(), "\n";

/* clear() keeps the counters. */
var_dump($a->clear());
echo counts(), "\n";
?>
--EXPECT--
total 0/0 a 0/0 b 0/0
string(7) "default"
total 0/1 a 0/1 b 0/0
bool(true)
string(1) "v"
string(1) "v"
total 2/1 a 2/1 b 0/0
bool(true)
bool(false)
total 2/1 a 2/1 b 0/0
bool(true)
array(3) {
  ["k"]=>
  string(1) "v"
  ["m"]=>
  array(1) {
    ["x"]=>
    int(1)
  }
  ["nope"]=>
  NULL
}
total 4/2 a 4/2 b 0/0
string(8) "computed"
string(8) "computed"
total 5/3 a 4/2 b 1/1
bool(true)
string(7) "expired"
total 5/4 a 4/2 b 1/2
total 7/5 a 6/3 b 1/2
bool(true)
total 7/5 a 6/3 b 0/0
string(4) "gone"
total 7/6 a 6/3 b 0/1
bool(true)
total 7/6 a 6/3 b 0/1
