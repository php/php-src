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
var_dump(session_create_id("AB\0C\0D"));


?>
Done
--EXPECTF--
Warning: session_create_id(): Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d
bool(false)

Warning: session_create_id(): Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d
bool(false)

Warning: session_create_id(): Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d
bool(false)

Warning: session_create_id(): Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d
bool(false)
Done
