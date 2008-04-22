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

var_dump(ini_get("session.cache_expire"));
var_dump(session_cache_expire());
var_dump(ini_get("session.cache_expire"));
var_dump(session_cache_expire(999));
var_dump(ini_get("session.cache_expire"));
var_dump(session_start());
var_dump(session_cache_expire());
var_dump(ini_get("session.cache_expire"));
var_dump(session_destroy());
var_dump(session_cache_expire());
var_dump(ini_get("session.cache_expire"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_cache_expire() : variation ***
string(3) "180"
int(180)
string(3) "180"
int(180)
string(3) "999"
bool(true)
int(999)
string(3) "999"
bool(true)
int(999)
string(3) "999"
Done
--UEXPECTF--
*** Testing session_cache_expire() : variation ***
unicode(3) "180"
int(180)
unicode(3) "180"
int(180)
unicode(3) "999"
bool(true)
int(999)
unicode(3) "999"
bool(true)
int(999)
unicode(3) "999"
Done

