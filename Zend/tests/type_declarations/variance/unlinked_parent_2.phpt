--TEST--
Using an unlinked parent interface
--FILE--
<?php

spl_autoload_register(function($class) {
    class X implements B {}
});

interface B extends A {
}

?>
--EXPECTF--
Fatal error: Interface 'B' not found in %s on line %d
