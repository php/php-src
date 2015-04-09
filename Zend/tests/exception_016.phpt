--TEST--
Exceptions on improper usage of $this
--FILE--
<?php
try {
	$this->foo();
} catch (EngineException $e) {
	echo "\nException: " . $e->getMessage() . " in " , $e->getFile() . " on line " . $e->getLine() . "\n";
}

$this->foo();
?>
--EXPECTF--
Exception: Using $this when not in object context in %sexception_016.php on line %d

Fatal error: Using $this when not in object context in %sexception_016.php on line %d
