--TEST--
Test session_cache_expire() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : int session_cache_expire([int $new_cache_expire])
 * Description : Return current cache expire
 * Source code : ext/session/session.c
 */

echo "*** Testing session_cache_expire() : variation ***\n";

ini_set("session.cache_expire", 360);
var_dump(session_cache_expire());
var_dump(session_cache_expire(1234567890));
var_dump(session_cache_expire(180));
var_dump(session_start());
var_dump(session_cache_expire());
var_dump(session_destroy());
var_dump(session_cache_expire());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_cache_expire() : variation ***
int(360)
int(360)
int(1234567890)
bool(true)
int(180)
bool(true)
int(180)
Done
