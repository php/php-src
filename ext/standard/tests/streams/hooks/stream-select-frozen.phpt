--TEST--
IO hooks: stream_select() freezes the streams of its sets while it waits
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);
[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

$scheduler->spawn(function () use ($a, $b) {
    /* The same stream in two sets is frozen once */
    $r = [$a];
    $w = null;
    $e = [$a];
    var_dump(stream_select($r, $w, $e, 10));
    var_dump(count($r));
    fclose($a);
    echo "A closed\n";
});
$scheduler->spawn(function () use ($a, $b) {
    foreach ([
        'fclose' => fn () => fclose($a),
        'fread' => fn () => fread($a, 1),
        'stream_select' => function () use ($a) { $r = [$a]; $w = $e = null; stream_select($r, $w, $e, 0); },
    ] as $name => $fn) {
        try {
            $fn();
        } catch (Error $e) {
            echo "B $name: ", $e->getMessage(), "\n";
        }
    }
    fwrite($b, "x");
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
B fclose: Concurrent access to a stream
B fread: Concurrent access to a stream
B stream_select: Concurrent access to a stream
int(1)
int(1)
A closed
