--TEST--
Wrong assertion
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo($transitions) {
	foreach ($transitions as $transition) {
		if (isEmpty()) {
			continue;
		}
	}
}
?>
OK
--EXPECT--
OK
