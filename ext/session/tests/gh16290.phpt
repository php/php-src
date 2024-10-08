--TEST--
GH-16290 (overflow on session cookie_lifetime ini)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
session_set_cookie_params(PHP_INT_MAX, '/', null, false, true);
?>
--EXPECTF--
Warning: session_set_cookie_params(): CookieLifetime must be between 0 and %d in %s on line %d
