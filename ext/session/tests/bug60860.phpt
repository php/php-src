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
Warning: session_start(): user session functions not defined in %s on line 3

Fatal error: session_start(): Failed to initialize storage module: user (path:%s) in %s on line 3
