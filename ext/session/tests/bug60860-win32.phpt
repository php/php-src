--TEST--
Bug #60860 (session.save_handler=user without defined function core dumps)
--SKIPIF--
<?php
include('skipif.inc');
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
?>
--INI--
session.save_handler=user
display_errors=off
--FILE--
<?php

session_start();
echo "ok\n";

?>
--EXPECTF--
ok
PHP Recoverable fatal error:  PHP Startup: Cannot set 'user' save handler by ini_set() or sesion_module_name() in Unknown on line 0

