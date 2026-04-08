--TEST--
Test session.cookie_samesite with invalid INI value
--INI--
session.cookie_samesite=Invalid
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

var_dump(ini_get("session.cookie_samesite"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
Warning: PHP Startup: session.cookie_samesite must be "Strict", "Lax", "None", or "" in Unknown on line 0
string(0) ""
Done
