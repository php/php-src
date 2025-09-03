--TEST--
Enum __wakeup
--FILE--
<?php

enum Foo {
    case Bar;

    public function __wakeup() {

    }
}

?>
--EXPECTF--
Deprecated: The __wakeup() serialization magic method has been deprecated. Implement __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Fatal error: Enum Foo cannot include magic method __wakeup in %s on line %d
