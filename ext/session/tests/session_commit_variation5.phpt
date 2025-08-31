--TEST--
Test session_commit() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_strict_mode=0
--FILE--
<?php

ob_start();

echo "*** Testing session_commit() : variation ***\n";

$id = md5(uniqid());
var_dump(session_id($id));
var_dump(session_start());
var_dump(session_id());
var_dump($id === session_id());
var_dump(session_commit());
var_dump($id === session_id());
var_dump(session_id());
var_dump(session_start());
var_dump($id === session_id());
var_dump(session_id());
var_dump(session_commit());
var_dump($id === session_id());
var_dump(session_id());
var_dump(session_start());
var_dump($id === session_id());
var_dump(session_id());
var_dump(session_commit());
var_dump($id === session_id());
var_dump(session_id());
var_dump(session_start());
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_commit() : variation ***
string(0) ""
bool(true)
string(32) "%s"
bool(true)
bool(true)
bool(true)
string(32) "%s"
bool(true)
bool(true)
string(32) "%s"
bool(true)
bool(true)
string(32) "%s"
bool(true)
bool(true)
string(32) "%s"
bool(true)
bool(true)
string(32) "%s"
bool(true)
bool(true)
Done
