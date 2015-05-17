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
Fatal error: Uncaught EngineException: Using $this when not in object context in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
