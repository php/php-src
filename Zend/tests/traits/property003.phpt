--TEST--
Conflicting properties with different visibility modifiers should result in a fatal error, since this indicates that the code is incompatible.
--FILE--
<?php

trait THello1 {
  public $hello;
}

trait THello2 {
  private $hello;
}

echo "PRE-CLASS-GUARD\n";

class TraitsTest {
	use THello1;
	use THello2;
}

echo "POST-CLASS-GUARD\n";

$t = new TraitsTest;
$t->hello = "foo";
?>
--EXPECTF--
PRE-CLASS-GUARD

Fatal error: THello1 and THello2 define the same property ($hello) in the composition of TraitsTest. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
