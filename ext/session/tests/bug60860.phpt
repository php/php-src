--TEST--
Bug #60860 (session.save_handler=user without defined function core dumps)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_handler=user
--FILE--
<?php

session_start();
echo "ok\n";

?>
--EXPECTF--
PHP Recoverable fatal error:  PHP Startup: Cannot set 'user' save handler by ini_set() or sesion_module_name() in Unknown on line 0

Recoverable fatal error: PHP Startup: Cannot set 'user' save handler by ini_set() or sesion_module_name() in Unknown on line 0
ok

