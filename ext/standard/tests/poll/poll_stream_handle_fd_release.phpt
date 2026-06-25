--TEST--
Io\Poll: StreamPollHandle releases its stream resource (no fd leak)
--SKIPIF--
<?php
if (!is_dir('/proc/self/fd')) {
    die("skip requires /proc/self/fd (Linux)\n");
}
?>
--FILE--
<?php
function open_fds(): int {
    return count(scandir('/proc/self/fd'));
}

$before = open_fds();
for ($i = 0; $i < 100; $i++) {
    list($r, $w) = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
    $h = new StreamPollHandle($r);
    unset($h, $r, $w);
}
gc_collect_cycles();
$delta = open_fds() - $before;

// Without the fix each handle pins its stream, leaking ~100 fds.
var_dump($delta < 10);
?>
--EXPECT--
bool(true)
