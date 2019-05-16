--TEST--
Test session_set_save_handler() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_set_save_handler(callback $open, callback $close, callback $read, callback $write, callback $destroy, callback $gc)
 * Description : Sets user-level session storage functions
 * Source code : ext/session/session.c
 */

echo "*** Testing session_set_save_handler() : variation ***\n";

var_dump(session_module_name());
var_dump(session_module_name(FALSE));
var_dump(session_module_name());
var_dump(session_module_name("blah"));
var_dump(session_module_name());
var_dump(session_module_name("files"));
var_dump(session_module_name());

ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : variation ***
string(%d) "%s"

Warning: session_module_name(): Cannot find named PHP session module () in %s on line %d
bool(false)
string(%d) "%s"

Warning: session_module_name(): Cannot find named PHP session module (blah) in %s on line %d
bool(false)
string(%d) "%s"
string(%d) "%s"
string(5) "files"
