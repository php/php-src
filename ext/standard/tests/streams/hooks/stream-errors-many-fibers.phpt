--TEST--
IO hooks: stream error operations of many suspended fibers do not add up to the depth limit
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip needs 3000 descriptors');
if (function_exists('posix_getrlimit')) {
    $hard = posix_getrlimit()['hard openfiles'];
    if ($hard !== 'unlimited' && $hard < 3200) die('skip needs 3200 descriptors');
}
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

if (function_exists('posix_getrlimit')) {
    $limits = posix_getrlimit();
    if ($limits['soft openfiles'] !== 'unlimited' && $limits['soft openfiles'] < 3200) {
        posix_setrlimit(POSIX_RLIMIT_NOFILE, 3200, $limits['hard openfiles'] === 'unlimited' ? -1 : $limits['hard openfiles']);
    }
}

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$n = 1500;
$pairs = [];
$results = [];
for ($i = 0; $i < $n; $i++) {
    $pairs[$i] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    $scheduler->spawn(function () use ($i, &$pairs, &$results) {
        $results[$i] = fread($pairs[$i][0], 16);
    });
}
$scheduler->spawn(function () use ($n, &$pairs) {
    for ($i = 0; $i < $n; $i++) {
        fwrite($pairs[$i][1], "r$i");
    }
});
$scheduler->loop();

$ok = 0;
for ($i = 0; $i < $n; $i++) {
    $ok += ($results[$i] ?? null) === "r$i";
}
var_dump($ok);
?>
--EXPECT--
int(1500)
