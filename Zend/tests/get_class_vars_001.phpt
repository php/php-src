--TEST--
get_class_vars(): Simple test
--FILE--
<?php

class A {
    public $a = 1;
    private $b = 2;
    private $c = 3;
}

class B extends A {
    static public $aa = 4;
    static private $bb = 5;
    static protected $cc = 6;
}


var_dump(get_class_vars('A'));
var_dump(get_class_vars('B'));

try {
    get_class_vars("Unknown");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
array(1) {
  ["a"]=>
  int(1)
}
array(2) {
  ["a"]=>
  int(1)
  ["aa"]=>
  int(4)
}
get_class_vars(): Argument #1 ($class) must be a valid class name, Unknown given
