--TEST--
Test session_destroy() function : variation4
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_destroy(void)
 * Description : lazy_destroy feature
 * Source code : ext/session/session.c
 */

echo "*** Testing session_destroy() : variation ***\n";

echo "Create new session\n";
var_dump(session_start());
$id = session_id();
var_dump($id);
$_SESSION['foo'] = 123;
var_dump(session_commit());

echo "Destroy session\n";
var_dump(session_id($id));
var_dump(session_start(['lazy_destroy'=>2]));
var_dump($_SESSION);
var_dump(session_destroy());

echo "Destroyed session must be aviable by lazy_destroy\n";
var_dump(session_id($id));
var_dump(session_start());
var_dump($_SESSION);
var_dump(session_commit());

sleep(3);

echo "Destroyed session must be gone now\n";
var_dump(session_id($id));
var_dump(session_start());
$new_id = session_id();
var_dump(session_id());
var_dump($_SESSION);
var_dump(session_commit());

echo "New session ID must be here\n";
var_dump($id !== $new_id);

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_destroy() : variation ***
Create new session
bool(true)
string(%d) "%s"
NULL
Destroy session
string(%d) "%s"
bool(true)
array(1) {
  ["foo"]=>
  int(123)
}
bool(true)
Destroyed session must be aviable by lazy_destroy
string(0) ""
bool(true)
array(2) {
  ["foo"]=>
  int(123)
  ["__PHP_SESSION_DESTROY__"]=>
  int(%d)
}
NULL
Destroyed session must be gone now
string(%d) "%s"
bool(true)
string(%d) "%s"
array(0) {
}
NULL
New session ID must be here
bool(true)
Done


