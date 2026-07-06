--TEST--
session.save_path and session.referer_check must not contain null bytes
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

var_dump(ini_set('session.save_path', "/tmp\0evil"));
var_dump(ini_set('session.referer_check', "example.com\0evil"));

ob_end_flush();
echo "Done";
?>
--EXPECTF--
Warning: ini_set(): "session.save_path" must not contain null bytes in %s on line %d
bool(false)

Warning: ini_set(): "session.referer_check" must not contain null bytes in %s on line %d
bool(false)
Done
