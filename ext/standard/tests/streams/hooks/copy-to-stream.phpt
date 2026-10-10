--TEST--
IO hooks: stream_copy_to_stream() from a socket waits through the provider
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no AF_UNIX socket pair on Windows');
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$s = new Scheduler();
Io\Hooks\set_hooks($s);

[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$out = fopen('php://temp', 'w+');
$file = tmpfile();

$s->spawn(function () use ($a, $out, $file) {
    echo "copy start\n";
    var_dump(stream_copy_to_stream($a, $file, 5));
    var_dump(stream_copy_to_stream($a, $out, 5));
    rewind($file);
    rewind($out);
    var_dump(stream_get_contents($file), stream_get_contents($out));
});
$s->spawn(function () use ($b) {
    echo "writer runs while the copy waits\n";
    fwrite($b, "hello");
    usleep(1000);
    fwrite($b, "world");
});
$s->loop();
?>
--EXPECT--
copy start
writer runs while the copy waits
int(5)
int(5)
string(5) "hello"
string(5) "world"
