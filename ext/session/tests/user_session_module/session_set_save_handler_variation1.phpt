--TEST--
Test session_set_save_handler() function : variation
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

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

Warning: session_module_name(): Session handler module "" cannot be found in %s on line %d
bool(false)
string(%d) "%s"

Warning: session_module_name(): Session handler module "blah" cannot be found in %s on line %d
bool(false)
string(%d) "%s"
string(%d) "%s"
string(5) "files"
