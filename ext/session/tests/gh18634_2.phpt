--TEST--
GH-18634 (Using pipe character in session variable key causes session data to be removed - explicit session write)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
session_start();
$_SESSION['foo|bar'] = 'value';
session_write_close()
?>
--EXPECTF--
Warning: session_write_close(): Failed to write session data. Data contains invalid key "foo|bar" in %s on line %d
