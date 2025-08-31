--TEST--
Non-backed enum errors when case has int expression value
--FILE--
<?php

enum Foo {
    case Bar = 1 + 1;
}

?>
--EXPECTF--
Fatal error: Case Bar of non-backed enum Foo must not have a value in %s on line %d
