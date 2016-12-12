--TEST--
Bug #73100 (session_destroy null dereference in ps_files_path_create)
--SKIPIF--
<?php
if (!extension_loaded('session')) die('skip session extension not available');
?>
--FILE--
<?php
ob_start();
var_dump(session_start());
session_module_name("user");
var_dump(session_destroy());
?>
===DONE===
--EXPECTF--
bool(true)

Warning: session_module_name(): Cannot change save handler module when session is active in %s on line 4
bool(true)
===DONE===
