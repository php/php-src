--TEST--
GH-16290 (overflow on session cookie_lifetime ini)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ob_start();
session_set_cookie_params(PHP_INT_MAX, '/', null, false, true);
echo "DONE";
ob_end_flush();
?>
--EXPECTF--
Warning: session_set_cookie_params(): session.cookie_lifetime must be between 0 and %d in %s on line %d
DONE
