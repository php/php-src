--TEST--
Bug #60860 (session.save_handler=user without defined function core dumps)
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
?>
--INI--
session.save_handler=user
error_log=
--FILE--
<?php

session_start();
echo "ok\n";

?>
--EXPECT--
Fatal error: PHP Startup: Session save handler "user" cannot be set by ini_set() in Unknown on line 0
ok
