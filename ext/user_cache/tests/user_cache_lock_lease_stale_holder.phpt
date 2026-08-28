--TEST--
UserCache\Cache: a holder whose lease expired cannot extend or release the lock after another process took the key over
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('lock-lease-stale-holder');
$cache->clear();

var_dump($cache->store('guarded', 'original'));
var_dump($cache->lock('guarded', 1));
sleep(2);

$pid = pcntl_fork();
if ($pid === 0) {
    /* The lease has expired, so the key can be taken over. */
    $cache->store('child-locked', $cache->lock('guarded'));
    while ($cache->fetch('parent-done') === null) {
        usleep(10000);
    }
    $cache->store('child-store', $cache->store('guarded', 'child-value'));
    $cache->store('child-unlock', $cache->unlock('guarded'));
    exit(0);
}

if ($pid > 0) {
    while ($cache->fetch('child-locked') === null) {
        usleep(10000);
    }

    echo "child lock after expiry\n";
    var_dump($cache->fetch('child-locked'));

    /* The stale holder may neither extend the new holder's lease ... */
    echo "stale lease extension\n";
    var_dump($cache->lock('guarded', 5));

    /* ... nor release it: unlock() only drops the request-local bookkeeping,
     * so a fresh lock() attempt must still see the child's record. */
    echo "stale unlock\n";
    var_dump($cache->unlock('guarded'));
    var_dump($cache->lock('guarded'));

    $cache->store('parent-done', true);
    pcntl_waitpid($pid, $status);

    echo "child store and unlock\n";
    var_dump($cache->fetch('child-store'));
    var_dump($cache->fetch('child-unlock'));
    var_dump($cache->fetch('guarded'));

    echo "lock after child release\n";
    var_dump($cache->lock('guarded'));
    var_dump($cache->unlock('guarded'));
} else {
    echo "pcntl_fork() failed\n";
}
?>
--EXPECT--
bool(true)
bool(true)
child lock after expiry
bool(true)
stale lease extension
bool(false)
stale unlock
bool(true)
bool(false)
child store and unlock
bool(true)
bool(true)
string(11) "child-value"
lock after child release
bool(true)
bool(true)
