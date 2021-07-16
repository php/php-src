--TEST--
errmsg: Non-abstract method must contain body
--FILE--
<?php

abstract class test {
}

class Impl extends Test {
    function Foo();
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method Impl::Foo() must contain body in %s on line %d
