--TEST--
GH-20177: Access overridden private property in get_object_vars()
--FILE--
<?php

class A {
    private $prop = 'A::$prop';

    public function __construct() {
        var_dump(get_object_vars($this));
    }
}

class B extends A {
    protected $prop = 'B::$prop';
}

new B;

?>
--EXPECT--
array(1) {
  ["prop"]=>
  string(8) "A::$prop"
}
