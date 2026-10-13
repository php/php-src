--TEST--
IO hooks: an internal consumer of a stream frozen by another fiber gets the concurrent access Error
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);
[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

$scheduler->spawn(function () use ($a) {
    $data = fread($a, 10);
    echo "A read: $data\n";
});
$scheduler->spawn(function () use ($a, $b) {
    try {
        /* The source stream is fetched without the argument parsing check */
        file_put_contents('php://memory', $a);
    } catch (Error $e) {
        echo "B: ", $e->getMessage(), "\n";
    }
    fwrite($b, "hello");
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
B: Concurrent access to a stream
A read: hello
