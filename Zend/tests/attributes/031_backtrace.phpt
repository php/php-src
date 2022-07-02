--TEST--
Backtrace during attribute instance creation
--FILE--
<?php

#[Attribute]
class MyAttribute {
    public function __construct() {
        debug_print_backtrace();
        var_dump(debug_backtrace());
        var_dump((new Exception)->getTrace());
    }
}

#[MyAttribute]
class Test {}

(new ReflectionClass(Test::class))->getAttributes()[0]->newInstance();

?>
--EXPECTF--
#0 %s031_backtrace.php(12): MyAttribute->__construct()
#1 %s(%d): ReflectionAttribute->newInstance()
array(2) {
  [0]=>
  array(7) {
    ["file"]=>
    string(%d) "%s031_backtrace.php"
    ["line"]=>
    int(12)
    ["function"]=>
    string(11) "__construct"
    ["class"]=>
    string(11) "MyAttribute"
    ["object"]=>
    object(MyAttribute)#1 (0) {
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
  [1]=>
  array(7) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(11) "newInstance"
    ["class"]=>
    string(19) "ReflectionAttribute"
    ["object"]=>
    object(ReflectionAttribute)#2 (0) {
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
}
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(%d) "%s031_backtrace.php"
    ["line"]=>
    int(12)
    ["function"]=>
    string(11) "__construct"
    ["class"]=>
    string(11) "MyAttribute"
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
  [1]=>
  array(6) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(11) "newInstance"
    ["class"]=>
    string(19) "ReflectionAttribute"
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
}
