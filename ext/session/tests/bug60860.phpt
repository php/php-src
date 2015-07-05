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
Warning: session_start(): user session functions not defined in %s on line %d

Warning: session_start(): Cannot send session cache limiter - headers already sent (output started at %s:%d) in %s on line %d

Fatal error: Uncaught Error: Failed to initialize storage module: user (path: ) in %s:%d
Stack trace:
#0 %s(%d): session_start()
#1 {main}
  thrown in %s on line %d
