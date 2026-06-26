--TEST--
Errors: runtime type error when bound is violated
--FILE--
<?php
class Animal {}
function f<T : Animal>(T $x): T { return $x; }

try {
    f(42);
    echo "no error\n";
} catch (TypeError $e) {
    echo "type error\n";
}
?>
--EXPECT--
type error
