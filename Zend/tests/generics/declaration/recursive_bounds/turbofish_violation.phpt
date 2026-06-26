--TEST--
Recursive bounds: turbofish argument still must satisfy each parameter's bound
--FILE--
<?php
class Box<T> {}
class Pair<T: Box<U>, U: Box<T>> {}

try {
    $p = new Pair::<int, string>();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Type argument 1 to new Pair does not satisfy the bound Box<U> on parameter T, int given
