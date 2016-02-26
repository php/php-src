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

// Testing __PHP_SESSION__ key in $_SESSION

session_start();
$_SESSION['__PHP_SESSION__'] = 'bar';
var_dump(
	session_commit()
);

session_start();
var_dump(
	$_SESSION
);
session_commit();

// Cleanup
session_start();
session_destroy(true);
?>
--EXPECTF--
*** Testing timestamp : basic feature

Warning: session_commit(): Reserved key(__PHP_SESSION__) found. Removing the data in %s on line %d
NULL
array(0) {
}
