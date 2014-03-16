--TEST--
Test session_start() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.auto_start=1
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_start(void)
 * Description : Initialize session data
 * Source code : ext/session/session.c
 */

echo "*** Testing session_start() : variation ***\n";

var_dump(session_start(['lazy_write'=>TRUE]));
var_dump(session_id());
var_dump(session_destroy());
var_dump(session_id());

var_dump(session_start(['minimize_lock'=>TRUE]));
var_dump(session_id());
var_dump(session_destroy());
var_dump(session_id());

var_dump(session_start(['lazy_destroy'=>0]));
$id = session_id();
var_dump($id);
$_SESSION['foo'] = 'ABC';
var_dump(session_destroy());
var_dump(session_id($id));
var_dump(session_start());
var_dump($_SESSION);
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_start() : variation ***

Notice: A session had already been started - ignoring session_start() in %s on line 13
bool(true)
string(%d) "%s"
bool(true)
string(0) ""
bool(true)
string(%d) "%s"
bool(true)
string(0) ""
bool(true)
string(%d) "%s"
bool(true)
string(0) ""
bool(true)
array(0) {
}
bool(true)
Done

