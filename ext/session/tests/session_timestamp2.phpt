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

ini_set('session.ttl', 1);
ini_set('session.ttl_destroy', 0);
$now = time();

session_start();
$old_sid = session_id();
var_dump(session_info());
$old_info = session_info();
$_SESSION['foo'] = 'bar';
session_commit();

session_start();
var_dump($_SESSION);
session_commit();

sleep(2);

session_start();
var_dump($_SESSION);
session_commit();

session_start();
$new_sid = session_id();
var_dump($_SESSION);
var_dump(session_info());
$new_info = session_info();
session_destroy(true);
var_dump($old_sid, $new_sid, $old_sid !== $new_sid);

var_dump($old_info['CREATED'] !== $new_info['CREATED']);


session_id($old_sid);
session_start();
session_destroy(true);
?>
--EXPECTF--
*** Testing timestamp : basic feature
array(5) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["TTL"]=>
  int(%d)
  ["TTL_UPDATE"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
array(5) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["TTL"]=>
  int(%d)
  ["TTL_UPDATE"]=>
  int(%d)
  ["SIDS"]=>
  array(1) {
    int(%d)
  }
}
string(32) "%s"
string(32) "%s"
bool(true)
bool(true)
