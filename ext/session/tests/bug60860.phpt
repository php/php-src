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

Fatal error: Uncaught Exception: Failed to initialize storage module: user (path: ) in %s:3
Stack trace:
#0 %s(3): session_start()
#1 {main}
  thrown in %s on line 3