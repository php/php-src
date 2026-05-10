--TEST--
Errors: closure type parameter shadows enclosing function type parameter
--FILE--
<?php
function f<T>(): void {
    $cl = function <T>(): void {};
}
?>
--EXPECTF--
Fatal error: Type parameter T shadows enclosing type parameter in %s on line %d
