--TEST--
UserCache\Cache: while a live process pins a shared graph, recovery after a writer died inside a write section waits: stores fail, reads return the default and getStatus() still answers; once the pin is released the next operation recovers and empties the segment
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
$cache = UserCache\Cache::getPool('recovery-pin-blocked');
$cache->clear();
$cache->store('graph', ['a' => 1, 'b' => [1, 2, 3]]);
$cache->store('scalar', 42);

function report(UserCache\Cache $cache, string $phase): void
{
    $operations = [
        'store' => $cache->store("after-$phase", 1),
        'scalar' => $cache->fetch('scalar', 'MISS'),
        'graph' => $cache->fetch('graph', 'MISS'),
        'has' => $cache->has('scalar'),
        'lock' => $cache->lock("lock-$phase"),
    ];
    $status = UserCache\Cache::getStatus();
    echo $phase, ': ', json_encode($operations + [
        'availability' => $status->getAvailability()->name,
        'entries' => $status->getEntryCount(),
        'pins' => $status->getGraphPinnedReferences(),
    ]), "\n";
}

[$parent, $child] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$holder = pcntl_fork();
if ($holder === 0) {
    fclose($parent);
    $held = UserCache\Cache::getPool('recovery-pin-blocked')->fetch('graph');
    fwrite($child, $held['a'] === 1 ? 'P' : 'F');
    fread($child, 1);
    exit(0);
}
fclose($child);
var_dump(fread($parent, 1));

$crasher = pcntl_fork();
if ($crasher === 0) {
    putenv('USER_CACHE_DEBUG_EXIT_IN_WRITE_SECTION=1');
    UserCache\Cache::getPool('recovery-pin-blocked')->store('never', 'published');
    exit(1);
}
pcntl_waitpid($crasher, $status);
var_dump(pcntl_wexitstatus($status));

report($cache, 'blocked');

fwrite($parent, 'R');
pcntl_waitpid($holder, $status);

report($cache, 'recovered');
var_dump($cache->has('never'), $cache->fetch('after-recovered'));
?>
--EXPECT--
string(1) "P"
int(0)
blocked: {"store":false,"scalar":"MISS","graph":"MISS","has":false,"lock":false,"availability":"Available","entries":0,"pins":0}
recovered: {"store":true,"scalar":"MISS","graph":"MISS","has":false,"lock":true,"availability":"Available","entries":1,"pins":0}
bool(false)
int(1)
