--TEST--
Bug #77743: Incorrect pi node insertion for jmpznz with identical successors
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function buggy($a) {
	$id_country = $a;
	if ($id_country === false) {
		if (true) {
		}
	}
	var_dump($id_country);
}

buggy(42);
--EXPECT--
int(42)
