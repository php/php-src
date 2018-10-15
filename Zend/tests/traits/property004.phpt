--TEST--
Conflicting properties with different initial values are considered incompatible.
--FILE--
<?php
error_reporting(E_ALL);

trait THello1 {
  public $hello = "foo";
}

trait THello2 {
  private $hello = "bar";
}

echo "PRE-CLASS-GUARD\n";

class TraitsTest {
	use THello1;
	use THello2;
	public function getHello() {
	    return $this->hello;
	}
}

$t = new TraitsTest;
?>
--EXPECTF--
PRE-CLASS-GUARD

Fatal error: THello1 and THello2 define the same property ($hello) in the composition of TraitsTest. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
