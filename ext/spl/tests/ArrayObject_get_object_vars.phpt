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
--EXPECT--
array(0) {
}
array(1) {
  ["test"]=>
  NULL
}
