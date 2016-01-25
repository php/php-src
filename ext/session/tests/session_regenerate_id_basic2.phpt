--TEST--
Test session_regenerate_id() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.hash_function=1
session.hash_bits_per_character=5
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_regenerate_id([bool $delete_old_session])
 * Description : Update the current session id with a newly generated one
 * Source code : ext/session/session.c
 */

echo "*** Testing session_regenerate_id() : basic functionality ***\n";

var_dump(session_start());
$_SESSION['foo'] = 'bar';
$sids[] = session_id();
var_dump(session_id(), $_SESSION);
var_dump(session_regenerate_id());
$sids[] = session_id();
$_SESSION['bar'] = 'foo';
var_dump(session_id(), $_SESSION);
var_dump(session_regenerate_id());
$sids[] = session_id();
var_dump(session_id(), $_SESSION);
var_dump(@session_destroy(-1));
var_dump(session_id(), $_SESSION);

// Check SID uniquness
var_dump(
	$sids[0] !== $sids[1],
	$sids[1] !== $sids[2],
	$sids[2] !== $sids[0]
);

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_regenerate_id() : basic functionality ***
bool(true)
string(32) "%s"
array(1) {
  ["foo"]=>
  string(3) "bar"
}
bool(true)
string(32) "%s"
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["bar"]=>
  string(3) "foo"
}
bool(true)
string(32) "%s"
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["bar"]=>
  string(3) "foo"
}
bool(true)
string(0) ""
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["bar"]=>
  string(3) "foo"
}
bool(true)
bool(true)
bool(true)
Done