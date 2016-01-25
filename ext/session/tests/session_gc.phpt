--TEST--
Test session_gc() function : basic feature
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_path=
session.gc_probability=1
session.gc_divisor=1000000
session.gc_maxlifetime=0
--FILE--
<?php

ob_start();

echo "*** Testing session_gc() : basic feature\n";

var_dump(session_gc());

session_start();

var_dump(session_gc());

@session_destroy(-1);

?>
--EXPECTF--
*** Testing session_gc() : basic feature

Warning: session_gc(): Session is not active in %s on line 7
bool(false)
int(0)
