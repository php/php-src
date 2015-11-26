--TEST--
Test session_id() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.hash_function=0
session.entropy_file=
session.entropy_length=0
--FILE--
<?php

ob_start();

/* 
 * Prototype : string session_id([string $id])
 * Description : Get and/or set the current session id
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_id() : variation ***\n";

$directory = dirname(__FILE__);
$filename = ($directory."/entropy.txt");
var_dump(ini_set("session.entropy_file", $filename));
var_dump(file_put_contents($filename, "Hello World!"));
var_dump(ini_set("session.entropy_length", filesize($filename)));

var_dump(ini_set("session.hash_function", 0));
var_dump(session_id());
var_dump(session_start());
var_dump(session_id());
var_dump(session_destroy());

var_dump(ini_set("session.hash_function", 1));
var_dump(session_id());
var_dump(session_start());
var_dump(session_id());
var_dump(session_destroy());
var_dump(unlink($filename));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_id() : variation ***
string(0) ""
int(12)
string(1) "0"
string(1) "0"
string(0) ""
bool(true)
string(%d) "%s"
bool(true)
string(1) "0"
string(0) ""
bool(true)
string(%d) "%s"
bool(true)
bool(true)
Done

