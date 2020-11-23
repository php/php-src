--TEST--
Test session_cache_limiter() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_cache_limiter() : variation ***\n";

ini_set("session.cache_limiter", "nocache");
var_dump(session_cache_limiter());
var_dump(session_start());
var_dump(session_cache_limiter());
var_dump(session_cache_limiter("public"));
var_dump(session_cache_limiter());
var_dump(session_destroy());
var_dump(session_cache_limiter());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_cache_limiter() : variation ***
string(7) "nocache"
bool(true)
string(7) "nocache"

Warning: session_cache_limiter(): Session cache limiter cannot be changed when a session is active in %s on line %d
bool(false)
string(7) "nocache"
bool(true)
string(7) "nocache"
Done
