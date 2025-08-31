--TEST--
Test stream_isatty with redirected STDOUT
--SKIPIF--
<?php
if (getenv("SKIP_IO_CAPTURE_TESTS")) {
    die("skip I/O capture test");
}
?>
--CAPTURE_STDIO--
STDOUT
--FILE--
<?php
require __DIR__.'/stream_isatty.inc';
testToStdOut();
?>
--EXPECT--
STDIN (constant): bool(true)
STDIN (fopen): bool(true)
STDIN (php://fd/0): bool(true)
STDOUT (constant): bool(false)
STDOUT (fopen): bool(false)
STDOUT (php://fd/1): bool(false)
STDERR (constant): bool(true)
STDERR (fopen): bool(true)
STDERR (php://fd/2): bool(true)
Invalid stream (php://temp): bool(false)
Invalid stream (php://input): bool(false)
Invalid stream (php://memory): bool(false)
File stream: bool(false)
