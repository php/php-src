--TEST--
Compiler prevents `abstract` methods on enums classes (GH-16067)
--FILE--
<?php

enum Example {
    abstract public function foo();
}

?>
--EXPECTF--
Fatal error: Enum Example cannot contain abstract method Example::foo in %s on line 4
