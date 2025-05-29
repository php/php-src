--TEST--
Introducing new private variables of the same name in a subclass is ok, and does not lead to any output. That is consistent with normal inheritance handling. (relevant to #60536)
--FILE--
<?php

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
--EXPECT--
PRE-CLASS-GUARD
POST-CLASS-GUARD
POST-CLASS-GUARD2
