--TEST--
UserCache\Cache: a write to a key another process has locked waits for the lock, fails when the wait runs out (including deleteMultiple(), add() and remember()), and succeeds when the holder unlocks during the wait; reads do not wait and a leased lock outlives its holder
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--EXTENSIONS--
pcntl
--ENV--
USER_CACHE_DEBUG_SHORT_ENTRY_LOCK_WAIT=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
$cache = UserCache\Cache::getPool('lock-wait-budget');
$cache->clear();
$cache->store('counter', 1);
$cache->store('value', 'old');

function elapsed_ms(float $started): float
{
    return (hrtime(true) - $started) / 1e6;
}

[$parent, $child] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$pid = pcntl_fork();
if ($pid === 0) {
    fclose($parent);
    $holder = UserCache\Cache::getPool('lock-wait-budget');
    $locked = $holder->lock('value', 30) && $holder->lock('counter', 30) && $holder->lock('batch', 30);
    fwrite($child, $locked ? 'L' : 'F');
    fread($child, 1);
    usleep(100000);
    $holder->unlock('value');
    fread($child, 1);
    exit(0);
}
fclose($child);
var_dump(fread($parent, 1));

$started = hrtime(true);
var_dump($cache->store('counter', 2));
$waited = elapsed_ms($started);
var_dump($waited >= 250 && $waited < 5000);

var_dump($cache->increment('counter'), $cache->delete('batch'), $cache->storeMultiple(['batch' => 1, 'other' => 2]));
var_dump($cache->fetch('counter'), $cache->has('other'), $cache->fetch('value'));

echo "\ndeleteMultiple(), add() and remember() when the wait runs out:\n";
var_dump($cache->store('unlocked', 'kept'));
$started = hrtime(true);
var_dump($cache->deleteMultiple(['unlocked', 'batch']));
$waited = elapsed_ms($started);
var_dump($waited >= 250 && $waited < 5000, $cache->fetch('unlocked'));

$started = hrtime(true);
var_dump($cache->add('batch', 'added'));
$waited = elapsed_ms($started);
var_dump($waited >= 250 && $waited < 5000, $cache->has('batch'));

$calls = 0;
$started = hrtime(true);
var_dump($cache->remember('batch', function (string $key) use (&$calls): string {
    $calls++;

    return "computed $key";
}));
$waited = elapsed_ms($started);
var_dump($calls, $waited >= 500 && $waited < 10000, $cache->has('batch'));

echo "\nthe holder unlocks during the wait:\n";

fwrite($parent, 'R');
$started = hrtime(true);
var_dump($cache->store('value', 'new'));
$waited = elapsed_ms($started);
var_dump($waited >= 50 && $waited < 290);

fwrite($parent, 'X');
pcntl_waitpid($pid, $status);
/* A lock taken with a lease keeps guarding the key after its holder exits. */
var_dump($cache->fetch('value'), $cache->store('counter', 3), $cache->fetch('counter'));
?>
--EXPECT--
string(1) "L"
bool(false)
bool(true)
NULL
bool(false)
bool(false)
int(1)
bool(false)
string(3) "old"

deleteMultiple(), add() and remember() when the wait runs out:
bool(true)
bool(false)
bool(true)
string(4) "kept"
bool(false)
bool(true)
bool(false)
string(14) "computed batch"
int(1)
bool(true)
bool(false)

the holder unlocks during the wait:
bool(true)
bool(true)
string(3) "new"
bool(false)
int(1)
