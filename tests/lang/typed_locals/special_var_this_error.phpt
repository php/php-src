--TEST--
Typed local variables: typing $this is a compile error
--FILE--
<?php
class C {
    function m() {
        string $this = 1;
    }
}
?>
--EXPECTF--
Fatal error: Cannot declare a type for the special variable $this in %s on line %d
