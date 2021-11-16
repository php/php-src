--TEST--
Test session_regenerate_id() function : basic functionality
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
opcache.fast_shutdown=1
--FILE--
<?php
session_start();
define ("user", "foo");
var_dump(session_regenerate_id());
?>
--EXPECT--
bool(true)
