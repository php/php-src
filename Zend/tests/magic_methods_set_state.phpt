--TEST--
Testing __set_state() declaration with wrong modifier
--FILE--
<?php

class Foo {
    function __set_state($array)
    {
    }
}

?>
--EXPECTF--
Fatal error: Method Foo::__set_state() must be static in %s on line %d
