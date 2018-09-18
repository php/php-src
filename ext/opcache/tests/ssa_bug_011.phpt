--TEST--
Wrong assertion
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
