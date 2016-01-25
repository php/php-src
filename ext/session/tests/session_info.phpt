--TEST--
Test session_info() function : basic feature
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_path=
session.hash_function=0
session.hash_bits_per_character=4
--FILE--
<?php

ob_start();

echo "*** Testing session_info() : basic feature\n";

$now = time();
session_start();
var_dump(session_id());
var_dump(session_info());
$info = session_info();
session_commit();

// All should be true
var_dump(abs($now - $info['CREATED']) <= 1);
var_dump(($info['CREATED'] - $info['UPDATED']) === 0);
var_dump(($info['TTL'] - $info['CREATED']) == ini_get('session.ttl'));
var_dump(count($info['SIDS']) === 0);

session_start();
var_dump(session_id());
var_dump(session_info());
session_commit();

session_start();
var_dump(session_id());
var_dump(session_info());
session_commit();

session_start();
var_dump(session_regenerate_id());
var_dump(session_id());
var_dump(session_info());
session_commit();

for ($i = 0; $i < 10; $i++) {
	session_start();
	session_regenerate_id();
	session_commit();
}

session_start();
var_dump(session_id());
var_dump(session_info());
$tmp = session_info();
$old_info = $tmp;
session_commit();

unset($tmp['SIDS']);

session_start();
$new_info = session_info();
$new = serialize($new_info);
$old = serialize($old_info);
var_dump($new === $old, $new, $old);

@session_destroy(-1);

?>
--EXPECTF--
*** Testing session_info() : basic feature
string(32) "%s"
array(4) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["TTL"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
bool(true)
bool(true)
bool(true)
bool(true)
string(32) "%s"
array(4) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["TTL"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
string(32) "%s"
array(4) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["TTL"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
bool(true)
string(32) "%s"
array(4) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["TTL"]=>
  int(%d)
  ["SIDS"]=>
  array(1) {
    [0]=>
    string(32) "%s"
  }
}
string(32) "%s"
array(4) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["TTL"]=>
  int(%d)
  ["SIDS"]=>
  array(9) {
    [2]=>
    string(32) "%s"
    [3]=>
    string(32) "%s"
    [4]=>
    string(32) "%s"
    [5]=>
    string(32) "%s"
    [6]=>
    string(32) "%s"
    [7]=>
    string(32) "%s"
    [8]=>
    string(32) "%s"
    [9]=>
    string(32) "%s"
    [10]=>
    string(32) "%s"
  }
}
bool(true)
string(497) "a:4:{s:7:"CREATED";%s}"
string(497) "a:4:{s:7:"CREATED";%s}"

