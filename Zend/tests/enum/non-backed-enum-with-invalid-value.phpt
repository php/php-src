--TEST--
Non-backed enum errors when case has invalid value
--FILE--
<?php

enum Foo {
    case Bar = 3.141;
}

?>
--EXPECTF--
Fatal error: Case Bar of non-backed enum Foo must not have a value in %s on line %d
