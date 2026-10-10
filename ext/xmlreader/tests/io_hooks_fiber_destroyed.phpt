--TEST--
XMLReader under io_hooks: a fiber destroyed at shutdown while a read waits for input unwinds it
--EXTENSIONS--
xmlreader
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
?>
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

[$writer, $input] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
fwrite($writer, '<?xml version="1.0"?>');
$reader = XMLReader::fromStream($input);

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$fiber = new Fiber(function () use (&$reader) {
    try {
        $reader->read();
    } finally {
        echo "A: unwound\n";
    }
});
$fiber->start();
try {
    $reader->close();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
unset($reader, $input, $fiber);
echo "end\n";
?>
--EXPECT--
Attempt to use XMLReader while it is reading
end
A: unwound
