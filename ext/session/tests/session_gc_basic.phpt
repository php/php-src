--TEST--
Test session_gc() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.serialize_handler=php
session.save_handler=files
session.maxlifetime=782000
--FILE--
<?php
error_reporting(E_ALL);

ob_start();

/*
 * Prototype : int session_gc([int maxlifetime])
 * Description : Execute gc and return number of deleted data
 * Source code : ext/session/session.c
 */

echo "*** Testing session_gc() : basic functionality ***\n";

// Should fail. It requires active session.
var_dump(session_gc());

session_start();
// Should succeed with some number
var_dump(session_gc());
// Secound time must be int(0)
var_dump(session_gc());
session_write_close();

// Start&stop session to generate
session_start();
session_write_close();
session_start();
session_write_close();
session_start();
session_write_close();

session_start();
$ndeleted = session_gc(0); // Delete all
var_dump($ndeleted >= 3);

echo "Done".PHP_EOL;

?>
--EXPECTF--
*** Testing session_gc() : basic functionality ***

Warning: session_gc(): Trying to garbage collect without active session in %s on line 15
bool(false)
int(%d)
int(0)
bool(true)
Done
