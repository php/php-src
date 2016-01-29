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

// Testing expired session access

session_start();
$id_begin = session_id();
$_SESSION['foo'] = 'bar';
$data = session_encode();
$session = unserialize($data);
$session['__PHP_SESSION__']['CREATED'] = 0;
$session['__PHP_SESSION__']['UPDATED'] = 0;
$session['__PHP_SESSION__']['SIDS'] = array();
var_dump(
	$data,
	session_info(),
	// Set very old timestamp
	session_decode(serialize($session)),
	$_SESSION,
	session_info(),
	session_commit()
);

echo "--------------------------------\n";

session_start();
$id_end = session_id();
$data = session_encode();
var_dump(
	$data,
	session_info(),
	$_SESSION,
	session_commit()
);

var_dump(
	$id_begin !== $id_end
);

// Cleanup
session_start();
@session_destroy(true);
session_commit();
session_id($id_begin);
@session_destroy(true);
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
array(1) {
  ["foo"]=>
  string(3) "bar"
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
string(6) "a:0:{}"
array(3) {
  ["CREATED"]=>
  int(%d)
  ["UPDATED"]=>
  int(%d)
  ["SIDS"]=>
  array(0) {
  }
}
array(0) {
}
NULL
bool(true)
