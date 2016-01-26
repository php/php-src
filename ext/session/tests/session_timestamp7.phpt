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
$_SESSION['foo'] = 'bar';
$data = session_encode();
$session = unserialize($data);
$session['some']['var'] = 1234;
$session['__PHP_SESSION__']['CREATED'] = 0;
$session['__PHP_SESSION__']['UPDATED'] = 0;
$session['__PHP_SESSION__']['SIDS'] = array();
var_dump(
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
$session['some']['var'] = 1234;
$session['__PHP_SESSION__']['CREATED'] = 0;
$session['__PHP_SESSION__']['UPDATED'] = 0;
$session['__PHP_SESSION__']['SIDS'] = array('foo'=>'bar');
var_dump(
	session_info(),
	session_decode(serialize($session)),
	$_SESSION,
	session_info(),
	session_commit()
);
session_commit();

// Cleanup
session_start();
@session_destroy(-1);
?>
--EXPECTF--
*** Testing timestamp : basic feature
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
  int(0)
  ["UPDATED"]=>
  int(0)
  ["SIDS"]=>
  array(0) {
  }
}
NULL
--------------------------------

Warning: session_decode(): Broken internal session data detected. Broken data has been wiped in %s on line %d
array(3) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["SIDS"]=>
  array(1) {
    [0]=>
    string(32) "%s"
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

