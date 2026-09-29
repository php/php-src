--TEST--
UserCache\Cache: records primed by has(), delete() and counters observe other processes' writes, and a record does not keep a zero-copy string that a forked process may see freed
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
function in_child(callable $write): void
{
    $pid = pcntl_fork();
    if ($pid === 0) {
        $write(UserCache\Cache::getPool('cross'));
        exit(0);
    }
    pcntl_waitpid($pid, $status);
    if (pcntl_wexitstatus($status) !== 0) {
        die("child failed\n");
    }
}

/* The segment outlives the request (php --repeat): start from fresh pools. */
UserCache\Cache::deletePool('cross');
UserCache\Cache::deletePool('zero-copy-record');
$cache = UserCache\Cache::getPool('cross');

echo "has():\n";
$cache->store('h', 'value');
var_dump($cache->has('h'), $cache->has('h'));
in_child(fn($c) => $c->delete('h'));
var_dump($cache->has('h'), $cache->has('h'));
in_child(fn($c) => $c->store('h', 'child'));
var_dump($cache->has('h'), $cache->fetch('h'));
$cache->store('ht', 'ttl', 3600);
var_dump($cache->has('ht'));
in_child(fn($c) => $c->delete('ht'));
var_dump($cache->has('ht'));

echo "delete():\n";
$cache->store('d', [1, 2]);
var_dump($cache->fetch('d'), $cache->delete('d'), $cache->fetch('d', 'gone'), $cache->has('d'));
in_child(fn($c) => $c->store('d', 'child'));
var_dump($cache->fetch('d'), $cache->has('d'));
var_dump($cache->delete('never-stored'), $cache->fetch('never-stored', 'absent'));
in_child(fn($c) => $c->store('never-stored', 'child'));
var_dump($cache->fetch('never-stored', 'absent'));

echo "deleteMultiple():\n";
$cache->storeMultiple(['m1' => 1, 'm2' => 2]);
var_dump($cache->fetch('m1'), $cache->deleteMultiple(['m1', 'm2']), $cache->fetch('m1', 'gone'), $cache->has('m2'));
in_child(fn($c) => $c->store('m2', 'child'));
var_dump($cache->fetch('m1', 'gone'), $cache->fetch('m2'), $cache->has('m1'));

echo "counters:\n";
var_dump($cache->increment('n', 3), $cache->fetch('n'), $cache->has('n'));
in_child(fn($c) => $c->increment('n', 10));
var_dump($cache->fetch('n'), $cache->decrement('n', 2), $cache->fetch('n'));
in_child(fn($c) => $c->decrement('n', 100));
var_dump($cache->fetch('n'));
in_child(fn($c) => $c->delete('n'));
var_dump($cache->fetch('n', 'gone'), $cache->increment('n'), $cache->fetch('n'));
var_dump($cache->increment('nt', 5, 3600), $cache->fetch('nt'), $cache->has('nt'));
in_child(fn($c) => $c->increment('nt', 1, 3600));
var_dump($cache->fetch('nt'), $cache->increment('nt', 1, 3600), $cache->fetch('nt'));
$cache->store('text', 'not a number');
var_dump($cache->fetch('text'));
try {
    $cache->increment('text');
} catch (Throwable $e) {
    echo get_class($e), "\n";
}
var_dump($cache->fetch('text'));

echo "rollback:\n";
$cache->store('r', 'old');
var_dump($cache->fetch('r'));
var_dump($cache->storeMultiple(['r' => 'new', 'huge' => str_repeat('x', 32 * 1024 * 1024)]));
var_dump($cache->fetch('r'));
in_child(function ($c) {
    if ($c->fetch('r') !== 'old') {
        exit(1);
    }
    $c->store('r', 'child');
});
var_dump($cache->fetch('r'));

echo "zero-copy string stored under another key:\n";
$result = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$pid = pcntl_fork();
if ($pid === 0) {
    fclose($result[0]);
    $c = UserCache\Cache::getPool('zero-copy-record');
    $c->store('arr', [str_repeat('A', 300), str_repeat('B', 300)]);
    $arr = $c->fetch('arr');
    $c->store('dst', $arr[0]);
    unset($arr);

    /* The grandchild waits until this process has ended its request and released its pins. */
    $exited = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    if (pcntl_fork() === 0) {
        fclose($exited[0]);
        stream_get_contents($exited[1]);
        $c->store('arr', 'replaced');
        for ($len = 780; $len >= 200 && UserCache\Cache::getStatus()->getWastedMemory() > 0; $len -= 4) {
            $c->store('fill' . $len, str_repeat('z', $len));
        }
        fwrite($result[1], $c->fetch('dst') === str_repeat('A', 300) ? 'intact' : 'corrupted');
        exit(0);
    }
    exit(0);
}
fclose($result[1]);
pcntl_waitpid($pid, $status);
var_dump(stream_get_contents($result[0]));
?>
--EXPECT--
has():
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
string(5) "child"
bool(true)
bool(false)
delete():
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
bool(true)
string(4) "gone"
bool(false)
string(5) "child"
bool(true)
bool(true)
string(6) "absent"
string(5) "child"
deleteMultiple():
int(1)
bool(true)
string(4) "gone"
bool(false)
string(4) "gone"
string(5) "child"
bool(false)
counters:
int(3)
int(3)
bool(true)
int(13)
int(11)
int(11)
int(-89)
string(4) "gone"
int(1)
int(1)
int(5)
int(5)
bool(true)
int(6)
int(7)
int(7)
string(12) "not a number"
ValueError
string(12) "not a number"
rollback:
string(3) "old"
bool(false)
string(3) "old"
string(5) "child"
zero-copy string stored under another key:
string(6) "intact"
