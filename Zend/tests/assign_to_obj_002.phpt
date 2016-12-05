--TEST--
Assign to $this leaks when $this not defined
--FILE--
<?php

try {
	$this->a = new stdClass;
} catch (Error $e) { echo $e->getMessage(), "\n"; }

?>
--EXPECT--
Using $this when not in object context
