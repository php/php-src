--TEST--
Bug #73100 (session_destroy null dereference in ps_files_path_create)
--SKIPIF--
<?php
if (!extension_loaded('session')) die('skip session extension not available');
?>
--INI--
session.save_path=
session.save_handler=files
--FILE--
<?php
ob_start();
var_dump(session_start());
session_module_name("user");
var_dump(session_destroy());

session_module_name("user");
?>
===DONE===
--EXPECTF--
bool(true)

Warning: session_module_name(): Cannot change save handler module when session is active in %s on line 4
bool(true)

Recoverable fatal error: session_module_name(): Cannot set 'user' save handler by ini_set() or session_module_name() in %s on line 7
