--TEST--
Test session_name() function : null byte in session name
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

var_dump(session_name("AB\0CD"));
var_dump(session_start());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
Fatal error: Uncaught ValueError: session_name(): Argument #1 ($name) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): session_name('AB\x00CD')
#1 {main}
  thrown in %s on line %d
