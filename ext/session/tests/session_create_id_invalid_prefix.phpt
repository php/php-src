--TEST--
Test session_create_id() function : invalid prefix
--INI--
session.save_handler=files
session.sid_length=32
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

var_dump(session_create_id('_'));
var_dump(session_create_id('%'));
var_dump(session_create_id("AB\0CD"));


?>
Done
--EXPECTF--
Warning: session_create_id(): Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d
bool(false)

Warning: session_create_id(): Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d
bool(false)

Fatal error: Uncaught ValueError: session_create_id(): Argument #1 ($prefix) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(5): session_create_id('AB\x00CD')
#1 {main}
  thrown in %s
