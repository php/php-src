--TEST--
Infinite recursion in unlinked_instanceof()
--FILE--
<?php
interface I {}
spl_autoload_register(function() {
    class X {
        function test(): I {}
    }
    class Y extends X {
        function test(): C {}
    }
});
class C extends Z implements C {}
?>
--EXPECTF--
Fatal error: Declaration of Y::test(): C must be compatible with X::test(): I in %s on line %d
