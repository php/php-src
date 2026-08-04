--TEST--
session.cookie_path, session.cookie_domain, and session.cache_limiter must not contain null bytes
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

var_dump(ini_set('session.cookie_path', "/path\0evil"));
var_dump(ini_set('session.cookie_domain', "example\0evil.com"));
var_dump(ini_set('session.cache_limiter', "nocache\0evil"));

var_dump(session_set_cookie_params(0, "/path\0evil"));
var_dump(session_set_cookie_params(0, null, "example\0evil.com"));

ob_end_flush();
echo "Done";
?>
--EXPECTF--
Warning: ini_set(): "session.cookie_path" must not contain null bytes in %s on line %d
bool(false)

Warning: ini_set(): "session.cookie_domain" must not contain null bytes in %s on line %d
bool(false)

Warning: ini_set(): "session.cache_limiter" must not contain null bytes in %s on line %d
bool(false)

Warning: session_set_cookie_params(): "session.cookie_path" must not contain null bytes in %s on line %d
bool(false)

Warning: session_set_cookie_params(): "session.cookie_domain" must not contain null bytes in %s on line %d
bool(false)
Done
