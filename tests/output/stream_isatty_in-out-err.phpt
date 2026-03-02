--TEST--
Test stream_isatty with redirected STDIN/STDOUT/STDERR
--SKIPIF--
<?php
if (getenv("SKIP_IO_CAPTURE_TESTS")) {
    die("skip I/O capture test");
}
?>
--CAPTURE_STDIO--
STDIN STDOUT STDERR
--FILE--
<?php
require __DIR__.'/stream_isatty.inc';
testToStdOut();
?>
--EXPECT--
STDIN (constant): bool(false)
STDIN (fopen): bool(false)
STDIN (php://fd/0): bool(false)
STDOUT (constant): bool(false)
STDOUT (fopen): bool(false)
STDOUT (php://fd/1): bool(false)
STDERR (constant): bool(false)
STDERR (fopen): bool(false)
STDERR (php://fd/2): bool(false)
Invalid stream (php://temp): bool(false)
Invalid stream (php://input): bool(false)
Invalid stream (php://memory): bool(false)
File stream: bool(false)
