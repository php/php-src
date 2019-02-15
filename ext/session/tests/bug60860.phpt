--TEST--
Bug #60860 (session.save_handler=user without defined function core dumps)
--SKIPIF--
<?php
include('skipif.inc');
?>
--INI--
session.save_handler=user
display_errors=off
error_log=
--FILE--
<?php

session_start();
echo "ok\n";

?>
--EXPECT--
PHP Recoverable fatal error:  PHP Startup: Cannot set 'user' save handler by ini_set() or session_module_name() in Unknown on line 0
ok
