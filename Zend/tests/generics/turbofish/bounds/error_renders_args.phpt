--TEST--
Bound error message: NAMED_WITH_ARGS bound is rendered with its type arguments
--FILE--
<?php
interface Comparable<T> {}

class Sorter<T : Comparable<T>> {}

try {
    new Sorter::<int>;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Type argument 1 to new Sorter does not satisfy the bound Comparable<T> on parameter T, int given
