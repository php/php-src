--TEST--
Stringable is automatically implemented for __toString() declared with non-canonical case
--FILE--
<?php

class Test {
    public function __TOSTRING() {
        return "foo";
    }
}

var_dump(new Test instanceof Stringable);
var_dump((new ReflectionClass(Test::class))->getInterfaceNames());
var_dump((string) new Test);

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(10) "Stringable"
}
string(3) "foo"
