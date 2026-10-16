--TEST--
UserCache\Cache: first reads of keys written by another process stay exact while concurrent writers force the locked read paths
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('stream_socket_pair')) die('skip requires stream_socket_pair');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=64M
--FILE--
<?php
const POOLS = 100;
const KEYS = 100;
const WRITERS = 2;

/* Arrays, strings and scalars each take their own emit path */
function fresh_value(int $p, int $k): mixed
{
    return match ($p % 3) {
        0 => ['p' => $p, 'k' => $k, 'f' => $k / 4],
        1 => str_repeat("v$p:$k;", 20),
        default => $k % 2 ? $k * 1.5 : $p * 1000 + $k,
    };
}

/* Map the segment before forking so every process shares it */
UserCache\Cache::getPool('fresh-reads-init')->store('init', true);

/* Another process writes the keys: this process has no key records for them */
$pid = pcntl_fork();
if ($pid < 0) die("fork failed\n");
if ($pid === 0) {
    for ($p = 0; $p < POOLS; $p++) {
        $cache = UserCache\Cache::getPool("fresh-reads-$p");
        for ($k = 0; $k < KEYS; $k++) {
            if (!$cache->store("key-$k", fresh_value($p, $k))) exit(1);
        }
    }
    exit(0);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);

/* Writers keep entering write sections until told to stop */
$writers = [];
for ($i = 0; $i < WRITERS; $i++) {
    [$parent, $child] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
    $pid = pcntl_fork();
    if ($pid < 0) die("fork failed\n");
    if ($pid === 0) {
        fclose($parent);
        $cache = UserCache\Cache::getPool("fresh-reads-writer-$i");
        $pad = str_repeat(chr(65 + $i), 1500);
        fwrite($child, 'r');
        stream_set_blocking($child, false);
        for ($n = 0; ; $n++) {
            if (!$cache->store('w' . ($n % 32), [$n, $pad])) exit(1);
            if ($n % 16 === 0 && fread($child, 1) === 'q') {
                exit($cache->store('last', $n) ? 0 : 2);
            }
        }
    }
    fclose($child);
    $writers[$pid] = $parent;
}
foreach ($writers as $socket) {
    if (fread($socket, 1) !== 'r') die("writer did not start\n");
}

$bad = 0;
for ($p = 0; $p < POOLS; $p++) {
    $cache = UserCache\Cache::getPool("fresh-reads-$p");
    for ($k = 0; $k < KEYS; $k++) {
        $expected = fresh_value($p, $k);
        switch ($k % 4) {
            case 0:
                $ok = $cache->fetch("key-$k") === $expected;
                break;
            case 1:
                $ok = $cache->has("key-$k") && !$cache->has("absent-$k");
                break;
            case 2:
                $ok = $cache->fetch("absent-$k", 'DEFAULT') === 'DEFAULT'
                    && $cache->fetch("key-$k") === $expected;
                break;
            default:
                $ok = $cache->fetchMultiple(["key-$k", "absent-$k"], false) === [
                    "key-$k" => $expected,
                    "absent-$k" => false,
                ];
                break;
        }
        if (!$ok) {
            $bad++;
        }
    }
}
var_dump($bad);

$exits = [];
foreach ($writers as $pid => $socket) {
    fwrite($socket, 'q');
    pcntl_waitpid($pid, $status);
    $exits[] = pcntl_wifexited($status) ? pcntl_wexitstatus($status) : -1;
    fclose($socket);
}
var_dump($exits);
for ($i = 0; $i < WRITERS; $i++) {
    var_dump(is_int(UserCache\Cache::getPool("fresh-reads-writer-$i")->fetch('last')));
}
?>
--EXPECT--
bool(true)
int(0)
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
bool(true)
bool(true)
