--TEST--
Covariant replacement of iterable type with unregistered intersection type in DNF type
--FILE--
<?php

class Test {
    function method1(): iterable|string {}
    function method2(): iterable|int {}
}
class Test2 extends Test {
    function method1(): (X&Traversable)|string {}
    function method2(): int|(X&MyIterator) {}
}

?>
===DONE===
--EXPECTF--
Fatal error: Could not check compatibility between Test2::method2(): (X&MyIterator)|int and Test::method2(): Traversable|array|int, because class X is not available in %s
