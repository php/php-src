--TEST--
UserCache\Cache: a process that finds every reader slot held by a live process stops probing the slot owners for the rest of the request
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!is_executable('/bin/sleep')) {
    die('skip needs /bin/sleep');
}
if (!is_readable('/proc/self/io')) {
    die('skip requires /proc/self/io');
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
const HOLDERS = 256;

function read_calls(): int
{
    preg_match('/syscr: (\d+)/', file_get_contents('/proc/self/io'), $m);

    return (int) $m[1];
}

$cache = UserCache\Cache::getPool('reader-slot-exhaustion');
$cache->clear();

$value = ['list' => range(1, 40), 'text' => str_repeat('p', 400)];
$cache->store('graph', $value);

[$ready_read, $ready_write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

/* Every holder claims a reader slot with its first fetch and then keeps it alive in sleep. */
$children = [];
for ($i = 0; $i < HOLDERS; $i++) {
    $pid = pcntl_fork();
    if ($pid === 0) {
        fclose($ready_read);
        $held = UserCache\Cache::getPool('reader-slot-exhaustion')->fetch('graph');
        fwrite($ready_write, $held === $value ? 'y' : 'n');
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

/* The first fetch probes the owners once; the following fetches reuse that answer. */
var_dump($cache->fetch('graph') === $value);

$calls = read_calls();
$same = true;
for ($i = 0; $i < 20; $i++) {
    $same = $same && $cache->fetch('graph') === $value;
}
$probes = read_calls() - $calls;
var_dump($same, $probes < 40);

foreach ($children as $pid) {
    posix_kill($pid, SIGKILL);
    pcntl_waitpid($pid, $child_status);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
