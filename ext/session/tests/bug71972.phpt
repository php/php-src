--TEST--
Bug #71972 (Cyclic references causing session_start(): Failed to decode session object)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_handler=files
--FILE--
<?php
ob_start();
session_start();

$_SESSION['boogie'] = 1;

$_SESSION['obj1'] = new stdClass();
for ( $x=2; $x < 20; $x++) {
	cyclic_ref($x);
}

function cyclic_ref($num) {
	$_SESSION['obj'.$num] = new stdClass();
	$_SESSION['obj'.$num]->test = new stdClass();//NOTE: No bug if try commenting out this too.
	$_SESSION['obj'.$num]->obj1 = $_SESSION['obj1'];
}

var_dump(session_decode(session_encode()) == $_SESSION);
?>
--EXPECT--
bool(true)
