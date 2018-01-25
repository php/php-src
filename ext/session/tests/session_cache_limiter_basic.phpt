--TEST--
Test session_cache_limiter() function : basic functionality
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

echo "*** Testing session_cache_limiter() : basic functionality ***\n";

var_dump(session_cache_limiter());
var_dump(session_cache_limiter("public"));
var_dump(session_cache_limiter());
var_dump(session_start());
var_dump(session_destroy());

var_dump(session_cache_limiter());
var_dump(session_cache_limiter("private"));
var_dump(session_cache_limiter());
var_dump(session_start());
var_dump(session_destroy());

var_dump(session_cache_limiter());
var_dump(session_cache_limiter("nocache"));
var_dump(session_cache_limiter());
var_dump(session_start());
var_dump(session_destroy());

var_dump(session_cache_limiter());
var_dump(session_cache_limiter("private_no_expire"));
var_dump(session_cache_limiter());
var_dump(session_start());
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_cache_limiter() : basic functionality ***
string(7) "nocache"
string(7) "nocache"
string(6) "public"
bool(true)
bool(true)
string(6) "public"
string(6) "public"
string(7) "private"
bool(true)
bool(true)
string(7) "private"
string(7) "private"
string(7) "nocache"
bool(true)
bool(true)
string(7) "nocache"
string(7) "nocache"
string(17) "private_no_expire"
bool(true)
bool(true)
Done
