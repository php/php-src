--TEST--
Enum disallows destructor
--FILE--
<?php

enum Foo {
    public function __destruct() {}
}

?>
--EXPECTF--
Fatal error: Enum may not include __destruct in %s on line %d
