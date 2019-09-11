--TEST--
Using an unlinked parent class
--FILE--
<?php

spl_autoload_register(function($class) {
    class X extends B {}
});

class B extends A {
}

?>
--EXPECTF--
Fatal error: Class 'B' not found in %s on line %d
