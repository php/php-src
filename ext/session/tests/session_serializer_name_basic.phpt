--TEST--
Test session_serializer_name() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : mixed session_serializer_name([string name])
 * Description : Change/get serialize handler name
 * Source code : ext/session/session.c
 */

echo "*** Testing session_serializer_name() : basic functionality ***\n";

var_dump(session_serializer_name());
var_dump(session_serializer_name('php'));
var_dump(session_serializer_name('php_binary'));
var_dump(session_serializer_name('none'));
var_dump(session_serializer_name());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_serializer_name() : basic functionality ***
string(3) "php"
bool(true)
bool(true)

Warning: session_serializer_name(): Cannot find serialization handler 'none' in %s/session_serializer_name_basic.php on line 16
bool(false)
string(10) "php_binary"
Done

