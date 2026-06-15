--TEST--
Typed local variables: strict_types=1 throws TypeError on lossy compound assignment
--FILE--
<?php
declare(strict_types=1);
int $x = 1;
try {
    $x += 0.5;
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($x);
?>
--EXPECTF--
Cannot assign float to local variable $x of type int
int(1)
