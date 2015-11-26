--TEST--
session_unset() without a initialized session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
session_unset();
print "ok\n";
--EXPECT--
ok
