--TEST--
Io\Ring\Engine: pending operations beyond the completion queue size do not hold back later ones
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

// Each read waits with a deadline, an operation and its linked timeout, far more than the ring holds
$n = 300;
$pairs = [];
for ($i = 0; $i < $n; $i++) {
    $pairs[$i] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
    stream_set_timeout($pairs[$i][0], 10);
}

$scheduler = new Scheduler(new Io\Ring\Engine(1));
Io\Hooks\set_hooks($scheduler);
$result = null;
$timedOut = 0;
for ($i = 0; $i < $n; $i++) {
    // The last reader gets the data first and passes it down, so nothing completes before it
    $scheduler->spawn(function () use ($i, $pairs, &$result, &$timedOut) {
        $data = fread($pairs[$i][0], 10);
        $timedOut += (int) stream_get_meta_data($pairs[$i][0])['timed_out'];
        if ($i > 0) {
            fwrite($pairs[$i - 1][1], $data);
        } else {
            $result = $data;
        }
    });
}
$scheduler->spawn(function () use ($n, $pairs) {
    fwrite($pairs[$n - 1][1], "relay");
});
$scheduler->loop();
var_dump($result, $timedOut);
?>
--EXPECT--
string(5) "relay"
int(0)
