--TEST--
Division by zero in compound operator with type coercion
--FILE--
<?php
$x = 42;
try {
    $$x /= 0;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $42 in %s on line %d
DivisionByZeroError: Division by zero
