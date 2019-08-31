--TEST--
Bug #31454 (session_set_save_handler crashes PHP when supplied non-existent object ref)
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

session_set_save_handler(
	array(&$arf, 'open'),
	array(&$arf, 'close'),
	array(&$arf, 'read'),
	array(&$arf, 'write'),
	array(&$arf, 'destroy'),
	array(&$arf, 'gc'));

echo "Done\n";
?>
--EXPECTF--
Warning: session_set_save_handler(): Argument 1 is not a valid callback in %sbug31454.php on line %d
Done
