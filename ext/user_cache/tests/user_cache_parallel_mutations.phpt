--TEST--
UserCache\Cache: concurrent increments, scalar overwrites and structural writes stay coherent with scalar stripes active and respect entry locks
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('stream_socket_pair')) die('skip requires stream_socket_pair');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
use UserCache\Cache;

$cache = Cache::getPool('parallel-mutations');
$workers = 4;
$iterations = 1000;
$cache->store('shared', 0);
$cache->store('shared', 0);
$cache->store('guarded', 0);
for ($worker = 0; $worker < $workers; $worker++) {
    $cache->store("own-$worker", 0);
}
if (!$cache->lock('guarded')) die("parent lock failed\n");

$pairs = [];
for ($worker = 0; $worker < $workers; $worker++) {
    $pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
    if ($pair === false) die("socket pair failed\n");
    stream_set_timeout($pair[0], 15);
    stream_set_timeout($pair[1], 15);
    $pairs[] = $pair;
}
$children = [];
for ($worker = 0; $worker < $workers; $worker++) {
    $pid = pcntl_fork();
    if ($pid < 0) die("fork failed\n");
    if ($pid === 0) {
        foreach ($pairs as $index => $pair) {
            fclose($pair[0]);
            if ($index !== $worker) fclose($pair[1]);
        }
        $peer = $pairs[$worker][1];
        if ($cache->unlock('guarded') || $cache->lock('guarded')) exit(1);
        fwrite($peer, "ready\n");
        if (fgets($peer) !== "go\n") exit(2);
        fwrite($peer, "attempting\n");
        if ($cache->increment('guarded') === null) exit(3);

        for ($i = 1; $i <= $iterations; $i++) {
            $number = $cache->increment('shared');
            if ($number === null || $cache->fetch('shared') < $number ||
                !$cache->store("own-$worker", $i) || $cache->fetch("own-$worker") !== $i) {
                exit(4);
            }
            if ($i % 32 === 0) {
                $text = str_repeat(chr(65 + $worker), 300) . $i;
                $array = [$worker, $i, ['text' => $text]];
                if (!$cache->store("text-$worker", $text) ||
                    !$cache->store("array-$worker", $array) ||
                    $cache->fetch("text-$worker") !== $text ||
                    $cache->fetch("array-$worker") !== $array) {
                    exit(5);
                }
                if (!$cache->delete("array-$worker") || $cache->has("array-$worker")) {
                    exit(6);
                }
            }
        }
        fclose($peer);
        exit(0);
    }
    $children[] = $pid;
}
foreach ($pairs as $pair) fclose($pair[1]);
foreach ($pairs as $pair) {
    if (fgets($pair[0]) !== "ready\n") die("ready barrier failed\n");
}
foreach ($pairs as $pair) fwrite($pair[0], "go\n");
foreach ($pairs as $pair) {
    if (fgets($pair[0]) !== "attempting\n") die("attempt barrier failed\n");
}
usleep(100000);
var_dump($cache->fetch('guarded') === 0);
var_dump($cache->unlock('guarded'));

$ok = true;
foreach ($children as $pid) {
    pcntl_waitpid($pid, $status);
    $ok = $ok && pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0;
}
foreach ($pairs as $pair) fclose($pair[0]);
var_dump($ok);
var_dump($cache->fetch('guarded') === $workers);
var_dump($cache->fetch('shared') === $workers * $iterations);
for ($worker = 0; $worker < $workers; $worker++) {
    if ($cache->fetch("own-$worker") !== $iterations) die("final overwrite mismatch\n");
}
echo "all private keys consistent\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
all private keys consistent
