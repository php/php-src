--TEST--
Stringable is automatically implemented
--FILE--
<?php

class Test {
    public function __toString() {
        return "foo";
    }
}

var_dump(new Test instanceof Stringable);
var_dump((new ReflectionClass(Test::class))->getInterfaceNames());

class Test2 extends Test {
    public function __toString() {
        return "bar";
    }
}

var_dump(new Test2 instanceof Stringable);
var_dump((new ReflectionClass(Test2::class))->getInterfaceNames());

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(10) "Stringable"
}
bool(true)
array(1) {
  [0]=>
  string(10) "Stringable"
}
