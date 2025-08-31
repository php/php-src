--TEST--
Enum disallows destructor
--FILE--
<?php

enum Foo {
    public function __destruct() {}
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __destruct in %s on line %d
