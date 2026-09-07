--TEST--
session_encode(): partial session data
--INI--
serialize_precision=100
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

session_start();

$_SESSION['data1'] = 'hello';
$_SESSION['data2'] = 'PHP';
$_SESSION['partial|data'] = 'key with pipe';
$_SESSION['data3'] = 'world';

var_dump(session_encode());

var_dump(session_destroy());

ob_end_flush();
?>
--EXPECTF--
Warning: session_encode(): Failed to write session data. Data contains invalid key "partial|data" in %s on line %d
bool(false)
bool(true)
