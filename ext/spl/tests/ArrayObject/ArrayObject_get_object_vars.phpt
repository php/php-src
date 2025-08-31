--TEST--
get_object_vars() on ArrayObject works on the properties of the ArrayObject itself
--FILE--
<?php

class Test {
    public $test;
    public $test2;
}

class AO extends ArrayObject {
    private $test;

    public function getObjectVars() {
        return get_object_vars($this);
    }
}

$ao = new AO(new Test);
var_dump(get_object_vars($ao));
var_dump($ao->getObjectVars());

?>
--EXPECTF--
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
array(0) {
}
array(1) {
  ["test"]=>
  NULL
}
