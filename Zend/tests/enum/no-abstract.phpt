--TEST--
Compiler prevents `abstract` methods on enums classes (GH-16067)
--FILE--
<?php

enum Example {
    abstract public function foo();
}

?>
--EXPECTF--
Fatal error: Enum method Example::foo() must not be abstract in %s on line 4
