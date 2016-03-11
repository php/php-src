--TEST--
Test timestamp : basic feature
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_path=
session.hash_function=0
session.hash_bits_per_character=4
--FILE--
<?php

ob_start();

echo "*** Testing timestamp : basic feature\n";

// Testing auto regenerate and destory

ini_set('session.ttl', 1);
ini_set('session.ttl_destroy', 0);
ini_set('session.ttl_update', 300);
ini_set('session.ttl_update', 300);
$now = time();

session_start();
$old_sid = session_id();
$old_info = session_info();
$_SESSION['foo'] = 'bar';
var_dump(
	$_SESSION,
	$old_info
);
session_commit();

sleep(2);

session_start();
$new_sid = session_id();
$new_info = session_info();
$_SESSION['bar'] = 'foo';
var_dump(
	$_SESSION,
	$new_info
);
session_destroy(true);

var_dump(
	$old_sid,
	$new_sid,
	$old_sid !== $new_sid,
	$old_info['CREATED'] !== $new_info['CREATED']
);

// Cleanup
session_id($old_sid);
session_start();
session_destroy(true);
?>
--EXPECTF--
*** Testing timestamp : basic feature
array(1) {
  ["foo"]=>
  string(3) "bar"
}
array(3) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
array(1) {
  ["bar"]=>
  string(3) "foo"
}
array(3) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
string(32) "%s"
string(32) "%s"
bool(true)
bool(true)
