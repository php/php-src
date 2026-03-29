--TEST--
Interface extending Invokable
--FILE--
<?php

interface MyInvokable extends Invokable {}

/* Class implementing MyInvokable must have __invoke */
class Good implements MyInvokable {
    public function __invoke(): void {}
}

var_dump(new Good() instanceof Invokable);
var_dump(new Good() instanceof MyInvokable);

var_dump((new ReflectionClass(Good::class))->getInterfaceNames());

?>
--EXPECT--
bool(true)
bool(true)
array(2) {
  [0]=>
  string(11) "MyInvokable"
  [1]=>
  string(9) "Invokable"
}
