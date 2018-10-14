--TEST--
Introducing new private variables of the same name in a subclass is ok, and does not lead to any output. That is consitent with normal inheritance handling. (relevant to #60536)
--FILE--
<?php
error_reporting(E_ALL | E_STRICT);

class Base {
  protected $hello;
}

trait THello1 {
  protected $hello;
}

// Protected and public are handle more strict with a warning then what is
// expected from normal inheritance since they can have easier coliding semantics
echo "PRE-CLASS-GUARD\n";
class SameNameInSubClassProducesNotice extends Base {
    use THello1;
}
echo "POST-CLASS-GUARD\n";

// now the same with a class that defines the property itself, too.

class Notice extends Base {
    use THello1;
    protected $hello;
}
echo "POST-CLASS-GUARD2\n";
?>
--EXPECTF--
PRE-CLASS-GUARD
POST-CLASS-GUARD
POST-CLASS-GUARD2
