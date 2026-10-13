--TEST--
IO hooks: a fatal error while a fiber is suspended in an operation leaves no stream frozen for the shutdown
--EXTENSIONS--
zlib
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);
[$r, $w] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

/* The deflate filter holds the data until the close flushes it */
stream_filter_append($r, 'zlib.deflate', STREAM_FILTER_WRITE);
fwrite($r, "pending");

$fiber = new Fiber(function () use ($r) {
    fread($r, 10);
    echo "unexpected\n";
});
$fiber->start();
echo "suspended\n";
eval('function strlen() {}');
?>
--EXPECTF--
suspended

Fatal error: Cannot redeclare function strlen() in %s : eval()'d code on line %d%A
