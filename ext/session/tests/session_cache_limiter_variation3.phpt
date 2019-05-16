--TEST--
Test session_cache_limiter() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : string session_cache_limiter([string $cache_limiter])
 * Description : Get and/or set the current cache limiter
 * Source code : ext/session/session.c
 */

echo "*** Testing session_cache_limiter() : variation ***\n";

var_dump(ini_get("session.cache_limiter"));
var_dump(session_start());
var_dump(ini_get("session.cache_limiter"));
var_dump(session_cache_limiter("public"));
var_dump(ini_get("session.cache_limiter"));
var_dump(session_destroy());
var_dump(ini_get("session.cache_limiter"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_cache_limiter() : variation ***
string(7) "nocache"
bool(true)
string(7) "nocache"

Warning: session_cache_limiter(): Cannot change cache limiter when session is active in %s on line 16
bool(false)
string(7) "nocache"
bool(true)
string(7) "nocache"
Done
