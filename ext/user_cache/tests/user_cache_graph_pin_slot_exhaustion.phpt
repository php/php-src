--TEST--
UserCache\Cache: a process without a graph pin slot decodes private copies instead of pinned views, also when a child forked during such a decode fetches other keys
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!is_executable('/bin/sleep')) {
    die('skip needs /bin/sleep');
}
if (getenv('USE_ZEND_ALLOC') === '0') {
    die('skip forks 256 processes under a memory checker');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=32M
--FILE--
<?php
use UserCache\Cache;

const HOLDERS = 256;

class Node
{
    public $name;
    public $items;

    public function __construct(string $name, array $items)
    {
        $this->name = $name;
        $this->items = $items;
    }
}

$cache = UserCache\Cache::getPool('graph-pin-slot-exhaustion');
$cache->clear();

$holder = ['held' => range(1, 8)];
$plain = ['list' => range(1, 40), 'text' => str_repeat('p', 400), 'nested' => ['k' => ['v' => 'w']]];
$objects = [new Node('a', ['x' => str_repeat('q', 300), 'y' => [1, 2, 3]]), new Node('b', [])];
$shared = ['leaf'];
for ($i = 0; $i < 40; $i++) {
    $shared = [$shared, $shared];
}
$cache->store('holder', $holder);
$cache->store('objects', $objects);
$cache->store('shared', new Node('shared', $shared));
$free_without_plain = UserCache\Cache::getStatus()->getFreeMemory();
$cache->store('plain', $plain);

[$ready_read, $ready_write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

/* Every holder pins a payload and then keeps its pin slot alive in sleep(1). */
$children = [];
for ($i = 0; $i < HOLDERS; $i++) {
    $pid = pcntl_fork();
    if ($pid === 0) {
        fclose($ready_read);
        $held = UserCache\Cache::getPool('graph-pin-slot-exhaustion')->fetch('holder');
        fwrite($ready_write, $held === $holder ? 'y' : 'n');
        fclose($ready_write);
        pcntl_exec('/bin/sleep', ['60']);
        exit(1);
    }
    $children[] = $pid;
}
fclose($ready_write);

$ready = '';
while (strlen($ready) < HOLDERS && !feof($ready_read)) {
    $ready .= fread($ready_read, HOLDERS);
}
var_dump($ready === str_repeat('y', HOLDERS));

$status = UserCache\Cache::getStatus();
var_dump($status->getGraphPinSlotsInUse(), $status->getGraphPinnedReferences());

for ($round = 0; $round < 3; $round++) {
    $fetched = $cache->fetch('plain');
    $nodes = $cache->fetch('objects');
    var_dump($fetched === $plain, $nodes == $objects);
}

/* A private copy keeps shared sub-arrays shared, so checking a later fetch visits each of them once. */
for ($round = 0; $round < 2; $round++) {
    $leaf = $cache->fetch('shared')->items;
    for ($i = 0; $i < 40; $i++) {
        $leaf = $leaf[$i % 2];
    }
    var_dump($leaf);
}

$status = UserCache\Cache::getStatus();
var_dump($status->getGraphPinSlotsInUse(), $status->getGraphPinnedReferences());

/* This process holds no reference: deleting the entry frees its payload. */
var_dump($cache->delete('plain'));
var_dump(UserCache\Cache::getStatus()->getFreeMemory() === $free_without_plain);

$fetched['list'][] = 'local';
var_dump(count($fetched['list']), $fetched['text'] === $plain['text']);
var_dump($nodes[0]->items['y'], $nodes[1]->name);

foreach ($children as $pid) {
    posix_kill($pid, SIGKILL);
    pcntl_waitpid($pid, $child_status);
}
sleep(1);

echo "\nfork during snapshot decode:\n";
$cache->clear();
$reclaim = Cache::getPool('graph-pin-dead-holders');
$half = str_repeat('r', intdiv(Cache::getStatus()->getFreeMemory(), 2) + 1000);
$reclaim->store('first', $half);
$reclaim->store('second', $half);
$reclaim->clear();
unset($half);
var_dump(Cache::getStatus()->getGraphPinSlotsInUse());

class ForkOnWakeup
{
    public $x = 1;

    public function __wakeup(): void
    {
        global $cache;

        $pid = pcntl_fork();
        if ($pid === 0) {
            $b = $cache->fetch('B', 'DEFAULT');
            echo 'child B: ', is_array($b) ? json_encode($b['list']) : var_export($b, true), "\n";
            echo 'child B still cached: ', var_export($cache->has('B'), true), "\n";
            exit(0);
        }
        pcntl_waitpid($pid, $status);
    }
}

$cache = Cache::getPool('pin-fork-snapshot');
$cache->clear();
$cache->store('A', ['obj' => new ForkOnWakeup]);
$cache->store('B', ['o' => new stdClass, 'list' => [1, 2, 3]]);
$cache->store('holder', ['held' => range(1, 8)]);

/* Children hold every graph pin slot, so this process decodes private copies. */
[$ready_read, $ready_write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
[$go_read, $go_write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$children = [];
for ($i = 0; $i < 256; $i++) {
    $pid = pcntl_fork();
    if ($pid === 0) {
        fclose($ready_read);
        $held = Cache::getPool('pin-fork-snapshot')->fetch('holder');
        fwrite($ready_write, is_array($held) ? 'y' : 'n');
        fclose($ready_write);
        if ($i === 0) {
            fread($go_read, 1);
            exit(0);
        }
        pcntl_exec('/bin/sleep', ['20']);
        exit(1);
    }
    $children[] = $pid;
}
fclose($ready_write);
$ready = '';
while (strlen($ready) < 256 && !feof($ready_read)) {
    $ready .= fread($ready_read, 256);
}
echo 'holders ready: ', var_export($ready === str_repeat('y', 256), true), "\n";

/* The parent fails to claim a slot; one holder then frees its slot for the forked child. */
$cache->fetch('holder');
fwrite($go_write, 'x');
pcntl_waitpid($children[0], $child_status);

$a = $cache->fetch('A');
echo 'parent A: ', get_debug_type($a['obj']), "\n";
foreach (array_slice($children, 1) as $pid) {
    posix_kill($pid, SIGKILL);
    pcntl_waitpid($pid, $child_status);
}
echo 'parent B: ', json_encode($cache->fetch('B', 'GONE')['list'] ?? 'GONE'), "\n";
?>
--EXPECT--
bool(true)
int(256)
int(256)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [0]=>
  string(4) "leaf"
}
array(1) {
  [0]=>
  string(4) "leaf"
}
int(256)
int(256)
bool(true)
bool(true)
int(41)
bool(true)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
string(1) "b"

fork during snapshot decode:
int(0)
holders ready: true
child B: [1,2,3]
child B still cached: true
parent A: ForkOnWakeup
parent B: [1,2,3]
