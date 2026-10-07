--TEST--
UserCache\Cache: a dead-pin sweep never mistakes a child taking over its parent's pins after pcntl_fork() for a dead owner
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--EXTENSIONS--
pcntl
posix
--ENV--
USER_CACHE_DEBUG_PAUSE_IN_DEAD_PIN_STRIP=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
display_errors=0
--FILE--
<?php
$cache = UserCache\Cache::getPool('fork-adoption-sweep');
$cache->clear();
$expected = ['name' => str_repeat('A', 300), 'list' => range(1, 64)];
$cache->store('cfg', $expected);
$cfg = $cache->fetch('cfg');

function wait_exit(int $pid): string
{
    pcntl_waitpid($pid, $status);

    return pcntl_wifsignaled($status) ? 'signal ' . pcntl_wtermsig($status) : 'exit ' . pcntl_wexitstatus($status);
}

function wait_for_file(string $path): string
{
    for ($i = 0; $i < 400; $i++) {
        clearstatcache(true, $path);
        if (filesize($path) > 0) {
            return file_get_contents($path);
        }
        usleep(25000);
    }

    return 'timeout';
}

function reclaimed(): int
{
    return UserCache\Cache::getStatus()->getDeadPinOwnersReclaimed();
}

echo "a sweep during the takeover leaves the child's pins alone:\n";
$go = tempnam(sys_get_temp_dir(), 'ucache_adopt_go');
$swept = tempnam(sys_get_temp_dir(), 'ucache_adopt_swept');
$result = tempnam(sys_get_temp_dir(), 'ucache_adopt_result');
$before = reclaimed();

/* The sibling's request-end release runs the dead-pin sweep. */
$sibling = pcntl_fork();
if ($sibling === 0) {
    wait_for_file($go);
    exit(0);
}

putenv('USER_CACHE_DEBUG_PAUSE_IN_FORK_ADOPTION=1');
$child = pcntl_fork();
if ($child === 0) {
    wait_for_file($swept);
    file_put_contents($result, $cfg === $expected ? 'kept' : 'changed');
    exit(0);
}
putenv('USER_CACHE_DEBUG_PAUSE_IN_FORK_ADOPTION');

usleep(500000);
file_put_contents($go, 'go');
echo "sibling ", wait_exit($sibling), "\n";
file_put_contents($swept, 'swept');
echo "child ", wait_exit($child), "\n";
echo "child value ", file_get_contents($result), "\n";
var_dump(reclaimed() - $before);
var_dump(UserCache\Cache::getStatus()->getGraphPinnedReferences());

echo "a takeover after a sweep found the parent dead fails:\n";
$forked = tempnam(sys_get_temp_dir(), 'ucache_adopt_forked');
file_put_contents($result, '');
$before = reclaimed();

$parent = pcntl_fork();
if ($parent === 0) {
    putenv('USER_CACHE_DEBUG_PAUSE_BEFORE_FORK_ADOPTION=1');
    $grandchild = pcntl_fork();
    if ($grandchild === 0) {
        file_put_contents($result, error_get_last()['message'] ?? 'adopted');
        exit(0);
    }
    file_put_contents($forked, 'forked');
    sleep(30);
    exit(0);
}

wait_for_file($forked);
posix_kill($parent, SIGKILL);
echo "parent ", wait_exit($parent), "\n";

$sweeper = pcntl_fork();
if ($sweeper === 0) {
    exit(0);
}
echo "sweeper ", wait_exit($sweeper), "\n";
echo "grandchild ", wait_for_file($result), "\n";
var_dump(reclaimed() - $before);
var_dump($cfg === $expected, $cache->fetch('cfg') === $expected);

foreach ([$go, $swept, $result, $forked] as $file) {
    unlink($file);
}
?>
--EXPECT--
a sweep during the takeover leaves the child's pins alone:
sibling exit 0
child exit 0
child value kept
int(0)
int(1)
a takeover after a sweep found the parent dead fails:
parent signal 9
sweeper exit 0
grandchild UserCache: values fetched before fork() could not be retained in the child process; the cache is disabled for the rest of this request
int(2)
bool(true)
bool(true)
