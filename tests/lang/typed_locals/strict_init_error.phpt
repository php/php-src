--TEST--
Typed local variables: strict_types=1 throws TypeError on non-coercible init
--FILE--
<?php
declare(strict_types=1);
try {
    int $n = "abc";
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Cannot assign string to local variable $n of type int
