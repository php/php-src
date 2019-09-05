--TEST--
Testing ftell() and fread() on closed std streams
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip not for Windows");
?>
--FILE--
<?php

shell_exec(PHP_BINARY . " -n " . __DIR__ . "/std_streams_closed.inc 0<&- 1>&- 2>&-");
echo file_get_contents(__DIR__ . '/std_streams_closed.txt');
unlink(__DIR__ . '/std_streams_closed.txt');

?>
--EXPECTF--
bool(false)
bool(false)
bool(false)

Warning: Trying to read from closed stdio stream in %s on line %d
bool(false)

Warning: Trying to write to closed stdio stream in %s on line %d
bool(false)

Warning: Trying to write to closed stdio stream in %s on line %d
bool(false)
