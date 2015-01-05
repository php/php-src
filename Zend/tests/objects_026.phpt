--TEST--
Using $this when out of context
--FILE--
<?php

try {
	$this->a = 1;
} catch (Exception $e) {
}

?>
--EXPECTF--
Fatal error: Using $this when not in object context in %s on line %d
