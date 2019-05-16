--TEST--
Test session_id() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : string session_id([string $id])
 * Description : Get and/or set the current session id
 * Source code : ext/session/session.c
 */

echo "*** Testing session_id() : basic functionality ***\n";

ini_set('session.sid_bits_per_chracter', 6);
ini_set('session.sid_length', 120);
session_start();
var_dump(session_id());
session_commit();

ini_set('session.sid_bits_per_chracter', 4);
ini_set('session.sid_length', 22);
session_start();
session_regenerate_id();
var_dump(session_id());
session_commit();

echo "Done";
?>
--EXPECTF--
*** Testing session_id() : basic functionality ***
string(120) "%s"
string(22) "%s"
Done
