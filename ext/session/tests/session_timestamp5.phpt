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

// Testing negative timestamp

ini_set('session.ttl', -1);
ini_set('session.ttl_destroy', -1);
ini_set('session.ttl_update', -1);
ini_set('session.regenerate_id', -1);
ini_set('session.num_sids', -1);
$now = time();

var_dump(
	ini_get('session.ttl'),
	ini_get('session.ttl_destroy'),
	ini_get('session.ttl_update'),
	ini_get('session.regenerate_id'),
	ini_get('session.num_sids')
);

session_start();
$old_sid = session_id();
var_dump(
	session_info()
);
$old_info = session_info();
session_commit();

sleep(2);

session_start();
$new_sid = session_id();
var_dump(
	session_info()
);
$new_info = session_info();
session_destroy(true);

var_dump(
	$old_sid,
	$new_sid,
	$old_sid === $new_sid,
	$old_info['CREATED'] === $new_info['CREATED'],
	$old_info['UPDATED'] === $new_info['UPDATED']
);

// Cleanup
session_id($old_sid);
session_start();
session_destroy(true);
?>
--EXPECTF--
*** Testing timestamp : basic feature
string(4) "1800"
string(2) "-1"
string(3) "300"
string(5) "64800"
string(1) "8"
array(3) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
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
bool(true)
