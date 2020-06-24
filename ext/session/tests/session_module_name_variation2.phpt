--TEST--
Test session_module_name() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_module_name() : variation ***\n";

function open($save_path, $session_name) { }
function close() { }
function read($id) { }
function write($id, $session_data) { }
function destroy($id) { }
function gc($maxlifetime) { }

var_dump(session_module_name("files"));
session_set_save_handler("open", "close", "read", "write", "destroy", "gc");
var_dump(session_module_name());

ob_end_flush();
?>
--EXPECTF--
*** Testing session_module_name() : variation ***
string(%d) "%s"
string(4) "user"
