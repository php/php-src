--TEST--
Test timestamp : basic feature
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.serialize_handler=php_serialize
session.save_path=
session.hash_function=0
session.hash_bits_per_character=4
--FILE--
<?php

ob_start();

echo "*** Testing timestamp : basic feature\n";

// Testing __PHP_SESSION__ array validation

session_start();
$id_begin = session_id();
$_SESSION['foo'] = 'bar';
$data = session_encode();
$session = unserialize($data);
$session['some']['var'] = 1234;
$session['__PHP_SESSION__']['CREATED'] = time();
$session['__PHP_SESSION__']['UPDATED'] = time();
$session['__PHP_SESSION__']['SIDS'] = array();
var_dump(
	$data,
	session_info(),
	session_decode(serialize($session)),
	$_SESSION,
	session_info(),
	session_commit()
);

echo "--------------------------------\n";

session_start();
$data = session_encode();
$session = unserialize($data);
$session['other']['var'] = 5678;
$session['__PHP_SESSION__']['CREATED'] = 0;
$session['__PHP_SESSION__']['UPDATED'] = 0;
$session['__PHP_SESSION__']['SIDS'] = array('foo'=>'bar');
var_dump(
	$data,
	session_info(),
	session_decode(serialize($session)),
	$_SESSION,
	session_info(),
	session_commit()
);

$id_end = session_id();
var_dump(
	$id_begin === $id_end
);

// Cleanup
@session_destroy(-1);
session_commit();
?>
--EXPECTF--
*** Testing timestamp : basic feature
string(26) "a:1:{s:3:"foo";s:3:"bar";}"
array(3) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
bool(true)
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["some"]=>
  array(1) {
    ["var"]=>
    int(1234)
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
NULL
--------------------------------

Warning: session_decode(): Broken internal session data detected. Broken internal session data has been wiped in %s on line %d
string(60) "a:2:{s:3:"foo";s:3:"bar";s:4:"some";a:1:{s:3:"var";i:1234;}}"
array(3) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
bool(true)
array(3) {
  ["foo"]=>
  string(3) "bar"
  ["some"]=>
  array(1) {
    ["var"]=>
    int(1234)
  }
  ["other"]=>
  array(1) {
    ["var"]=>
    int(5678)
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
NULL
bool(true)
