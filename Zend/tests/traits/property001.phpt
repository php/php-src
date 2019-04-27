--TEST--
"Potentially" conflicting trait properties do not result in a strict standards notice anymore
--FILE--
<?php
error_reporting(E_ALL);

trait THello1 {
  private $foo;
}

trait THello2 {
  private $foo;
}

echo "PRE-CLASS-GUARD-TraitsTest\n";

class TraitsTest {
	use THello1;
	use THello2;
}

echo "PRE-CLASS-GUARD-TraitsTest2\n";

class TraitsTest2 {
	use THello1;
	use THello2;
}

var_dump(property_exists('TraitsTest', 'foo'));
var_dump(property_exists('TraitsTest2', 'foo'));
?>
--EXPECT--
PRE-CLASS-GUARD-TraitsTest
PRE-CLASS-GUARD-TraitsTest2
bool(true)
bool(true)
