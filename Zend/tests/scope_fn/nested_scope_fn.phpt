--TEST--
Nested scope functions share CVs transitively with top-level parent
--FILE--
<?php
function outer() {
    $x = 10;
    $inner = fn($a) {
        $y = $a + $x;
        $innermost = fn($b) {
            return $b + $x + $y;
        };
        return $innermost(100);
    };
    var_dump($inner(5));
}
outer();
?>
--EXPECT--
int(125)
