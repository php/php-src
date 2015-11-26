--TEST--
Test session_start() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_start(void)
 * Description : Initialize session data
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_start() : variation ***\n";

var_dump(session_start());
var_dump(session_start());
var_dump(session_start());
var_dump(session_start());
var_dump(session_start());

session_destroy();
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_start() : variation ***
bool(true)

Notice: A session had already been started - ignoring session_start() in %s on line %d
bool(true)

Notice: A session had already been started - ignoring session_start() in %s on line %d
bool(true)

Notice: A session had already been started - ignoring session_start() in %s on line %d
bool(true)

Notice: A session had already been started - ignoring session_start() in %s on line %d
bool(true)
Done

