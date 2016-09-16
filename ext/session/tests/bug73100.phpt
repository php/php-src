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

Warning: session_module_name(): A session is active. You cannot change the session module's ini settings at this time in %s on line %d

Warning: session_destroy(): Session object destruction failed in %s on line %d
bool(false)
===DONE===
