--TEST--
Exceptions on improper usage of $this
--FILE--
<?php
try {
	$this->foo();
} catch (Error $e) {
	echo "\nException: " . $e->getMessage() . " in " , $e->getFile() . " on line " . $e->getLine() . "\n";
}

$this->foo();
?>
--EXPECTF--
Notice: Undefined variable: this in %s on line %d

Exception: Call to a member function foo() on null in %s on line %d

Notice: Undefined variable: this in %s on line %d

Fatal error: Uncaught Error: Call to a member function foo() on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
