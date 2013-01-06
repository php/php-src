--TEST--
Introducing new private variables of the same name in a subclass is ok, and does not lead to any output. That is consitent with normal inheritance handling. (relevant to #60536)
--FILE--
<?php
error_reporting(E_ALL | E_STRICT);

class Base {
  private $hello;    
}

trait THello1 {
  private $hello;
}

// Now we use the trait, which happens to introduce another private variable
// but they are distinct, and not related to each other, so no warning.
echo "PRE-CLASS-GUARD\n";
class SameNameInSubClassNoNotice extends Base {
    use THello1;
}
echo "POST-CLASS-GUARD\n";

// now the same with a class that defines the property itself,
// that should give the expected strict warning.

class Notice extends Base {
    use THello1;
    private $hello;
}
echo "POST-CLASS-GUARD2\n";
?>
--EXPECTF--	
PRE-CLASS-GUARD
POST-CLASS-GUARD

Strict Standards: Notice and THello1 define the same property ($hello) in the composition of Notice. This might be incompatible, to improve maintainability consider using accessor methods in traits instead. Class was composed in %sbug60536_004.php on line %d
POST-CLASS-GUARD2
