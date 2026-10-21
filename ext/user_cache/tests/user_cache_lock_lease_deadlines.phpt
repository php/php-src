--TEST--
UserCache\Cache: lease renewal, unbounded locks, deadlines past request end (also after re-locking with or without a lease), takeover after expiry and what a stale holder can still do
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
--FILE--
<?php
require __DIR__ . '/user_cache_clock.inc';

function other_process_locks(UserCache\Cache $cache, string $key): bool
{
    $pid = pcntl_fork();
    if ($pid === 0) {
        exit($cache->lock($key) ? 0 : 1);
    }
    pcntl_waitpid($pid, $status);

    return pcntl_wexitstatus($status) === 0;
}

$cache = UserCache\Cache::getPool('lease-deadlines');
$cache->clear();

echo "re-lock renews the lease\n";
var_dump($cache->lock('renewed', 1));
usleep(700000);
$renewed = hrtime();
var_dump($cache->lock('renewed', 1));
usleep(700000);
var_dump(!other_process_locks($cache, 'renewed') || seconds_since($renewed) >= 1);
$cache->unlock('renewed');

echo "a lease does not bound an unbounded lock\n";
var_dump($cache->lock('unbounded'));
var_dump($cache->lock('unbounded', 1));
usleep(1200000);
var_dump(other_process_locks($cache, 'unbounded'));
var_dump($cache->unlock('unbounded'));

echo "request end keeps the deadline set at lock()\n";
$pid = pcntl_fork();
if ($pid === 0) {
    $cache->store('left-at', hrtime());
    $cache->lock('left', 2);
    usleep(800000);
    exit(0);
}
pcntl_waitpid($pid, $status);
$left_at = $cache->fetch('left-at');
var_dump(!other_process_locks($cache, 'left') || seconds_since($left_at) >= 2);
usleep(max(0, (int) ((2.4 - seconds_since($left_at)) * 1000000)));
var_dump(other_process_locks($cache, 'left'));

echo "a lease blocks other processes until it expires, then passes to another process\n";
$taken_at = hrtime();
var_dump($cache->lock('taken', 1), $cache->lock('extended', 1));
var_dump(!other_process_locks($cache, 'taken') || seconds_since($taken_at) >= 1);
usleep(1200000);
$pid = pcntl_fork();
if ($pid === 0) {
    $cache->store('taken-ready', $cache->lock('taken') && $cache->lock('extended'));
    while ($cache->fetch('taken-done') === null) {
        usleep(10000);
    }
    $cache->store('taken-store', $cache->store('taken', 'new-holder'));
    $cache->store('taken-unlock', $cache->unlock('taken') && $cache->unlock('extended'));
    exit(0);
}
while (($taken = $cache->fetch('taken-ready')) === null) {
    usleep(10000);
}
var_dump($taken);

echo "unlock() after the lease passed to another process\n";
var_dump($cache->unlock('taken'));

echo "re-locking after the lease passed to another process does not extend it\n";
var_dump($cache->lock('extended', 5));
$cache->store('taken-done', true);
pcntl_waitpid($pid, $status);

echo "the new holder stores and unlocks\n";
var_dump($cache->fetch('taken-store'), $cache->fetch('taken-unlock'), $cache->fetch('taken'));
var_dump($cache->lock('taken'));
var_dump($cache->unlock('taken'));

echo "the stale holder exiting after the takeover leaves the new lock in place\n";
$pid = pcntl_fork();
if ($pid === 0) {
    $locked = $cache->lock('abandoned', 1);
    $cache->store('abandoned-at', hrtime());
    while ($cache->fetch('abandoned-done') === null) {
        usleep(10000);
    }
    exit($locked ? 0 : 1);
}
while (($abandoned_at = $cache->fetch('abandoned-at')) === null) {
    usleep(10000);
}
usleep(max(0, (int) ((1.2 - seconds_since($abandoned_at)) * 1000000)));
var_dump($cache->lock('abandoned'));
$cache->store('abandoned-done', true);
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status));
var_dump(other_process_locks($cache, 'abandoned'));
var_dump($cache->unlock('abandoned'));
var_dump(other_process_locks($cache, 'abandoned'));

echo "unlock() after the lease expired\n";
var_dump($cache->lock('expired', 1));
usleep(1200000);
var_dump($cache->unlock('expired'));
var_dump($cache->lock('expired'));
var_dump($cache->unlock('expired'));
echo "re-locking a leased lock without a lease keeps its deadline after the holder exits\n";
$pid = pcntl_fork();
if ($pid === 0) {
    $cache->store('relocked-at', hrtime());
    exit($cache->lock('relocked', 1) && $cache->lock('relocked') ? 0 : 1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status));
$relocked_at = $cache->fetch('relocked-at');
var_dump(!other_process_locks($cache, 'relocked') || seconds_since($relocked_at) >= 1);
usleep(max(0, (int) ((1.4 - seconds_since($relocked_at)) * 1000000)));
var_dump(other_process_locks($cache, 'relocked'));

echo "a lease added to an unbounded lock does not keep it after the holder exits\n";
$pid = pcntl_fork();
if ($pid === 0) {
    exit($cache->lock('unbounded-exit') && $cache->lock('unbounded-exit', 2) ? 0 : 1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status));
var_dump(other_process_locks($cache, 'unbounded-exit'));
?>
--EXPECT--
re-lock renews the lease
bool(true)
bool(true)
bool(true)
a lease does not bound an unbounded lock
bool(true)
bool(true)
bool(false)
bool(true)
request end keeps the deadline set at lock()
bool(true)
bool(true)
a lease blocks other processes until it expires, then passes to another process
bool(true)
bool(true)
bool(true)
bool(true)
unlock() after the lease passed to another process
bool(false)
re-locking after the lease passed to another process does not extend it
bool(false)
the new holder stores and unlocks
bool(true)
bool(true)
string(10) "new-holder"
bool(true)
bool(true)
the stale holder exiting after the takeover leaves the new lock in place
bool(true)
int(0)
bool(false)
bool(true)
bool(true)
unlock() after the lease expired
bool(true)
bool(false)
bool(true)
bool(true)
re-locking a leased lock without a lease keeps its deadline after the holder exits
int(0)
bool(true)
bool(true)
a lease added to an unbounded lock does not keep it after the holder exits
int(0)
bool(true)
