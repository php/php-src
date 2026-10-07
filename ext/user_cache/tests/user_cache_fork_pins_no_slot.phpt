--TEST--
A child that cannot take over the parent's pins after pcntl_fork() is warned and loses the cache for the rest of its request
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!is_executable('/bin/sleep')) {
    die('skip needs /bin/sleep');
}
if (getenv('USE_ZEND_ALLOC') === '0') {
    die('skip forks 255 processes under a memory checker');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
--FILE--
<?php

const HOLDERS = 255;

$pool = UserCache\Cache::getPool('fork-no-slot');
$expected = ['name' => str_repeat('A', 300), 'list' => range(1, 64)];
var_dump($pool->store('cfg', $expected));
var_dump($pool->store('holder', ['held' => range(1, 8)]));
$cfg = $pool->fetch('cfg');
var_dump(UserCache\Cache::getStatus()->getGraphPinnedReferences());

echo "every other pin slot is taken by a live process\n";
$children = [];
$ready = '';
for ($i = 0; $i < HOLDERS; $i++) {
    [$ready_read, $ready_write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    $pid = pcntl_fork();
    if ($pid === 0) {
        fclose($ready_read);
        $held = UserCache\Cache::getPool('fork-no-slot')->fetch('holder');
        fwrite($ready_write, $held['held'] === range(1, 8) ? 'y' : 'n');
        fclose($ready_write);
        pcntl_exec('/bin/sleep', ['60']);
        exit(1);
    }
    fclose($ready_write);
    $ready .= fread($ready_read, 1);
    fclose($ready_read);
    $children[] = $pid;
}
var_dump($ready === str_repeat('y', HOLDERS));
var_dump(UserCache\Cache::getStatus()->getGraphPinSlotsInUse());

echo "child without a pin slot\n";
$child = pcntl_fork();
if ($child === 0) {
    var_dump($pool->fetch('cfg'));
    var_dump(UserCache\Cache::getStatus()->getAvailability());
    var_dump($pool->store('from-child', 1));
    exit(0);
}
pcntl_waitpid($child, $status);
echo "child exit ", pcntl_wexitstatus($status), "\n";

echo "the parent keeps its pins\n";
var_dump($cfg === $expected);
var_dump(UserCache\Cache::getStatus()->getGraphPinnedReferences() === 1 + 2 * HOLDERS);
var_dump($pool->has('from-child'));

foreach ($children as $pid) {
    posix_kill($pid, SIGKILL);
    pcntl_waitpid($pid, $status);
}

?>
--EXPECTF--
bool(true)
bool(true)
int(1)
every other pin slot is taken by a live process
bool(true)
int(256)
child without a pin slot

Warning: UserCache: values fetched before fork() could not be retained in the child process; the cache is disabled for the rest of this request in %s on line %d
NULL
enum(UserCache\CacheAvailability::UnavailableByUnknownReason)
bool(false)
child exit 0
the parent keeps its pins
bool(true)
bool(true)
bool(false)
