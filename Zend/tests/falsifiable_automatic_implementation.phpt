--TEST--
Falsifiable is automatically implemented
--FILE--
<?php

class Test {
    public function __toBool() {
        return true;
    }
}

var_dump(new Test instanceof Falsifiable);
var_dump((new ReflectionClass(Test::class))->getInterfaceNames());

class Test2 extends Test {
    public function __toBool() {
        return false;
    }
}

var_dump(new Test2 instanceof Falsifiable);
var_dump((new ReflectionClass(Test2::class))->getInterfaceNames());

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(10) "Falsifiable"
}
bool(false)
array(1) {
  [0]=>
  string(10) "Falsifiable"
}
