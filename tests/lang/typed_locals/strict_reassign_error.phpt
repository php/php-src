--TEST--
Typed local variables: strict_types=1 throws TypeError on non-coercible reassignment
--FILE--
<?php
declare(strict_types=1);
int $x = 1;
try {
    $x = "5";
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Cannot assign string to local variable $x of type int
