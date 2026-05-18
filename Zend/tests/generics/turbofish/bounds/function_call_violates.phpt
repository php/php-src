--TEST--
Turbofish bound check: function call with a concrete arg violating the bound
--FILE--
<?php
class Animal {}
function id<T : Animal>(): void {}

try {
    id::<string>();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Type argument 1 to call id() does not satisfy the bound Animal on parameter T, string given
