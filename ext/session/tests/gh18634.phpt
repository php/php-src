--TEST--
GH-18634 (Using pipe character in session variable key causes session data to be removed)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
session_start();
$_SESSION['foo|bar'] = 'value';
?>
--EXPECT--
Warning: PHP Request Shutdown: Failed to write session data. Data contains invalid key "foo|bar" in Unknown on line 0
