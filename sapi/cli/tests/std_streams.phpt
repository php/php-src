--TEST--
Testing ftell() on std streams
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

// These have proc_open pipes attached
var_dump(ftell(STDIN));
var_dump(ftell(STDERR));
var_dump(ftell(fopen("php://stdin", "r")));
var_dump(ftell(fopen("php://stderr", "w")));

// These have a tty attached
var_dump(ftell(STDOUT));
var_dump(ftell(fopen("php://stdout", "w")));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
