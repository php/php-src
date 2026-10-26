--TEST--
GH-24708: Assertion failure in binary operators when an error handler unsets an operand
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    echo $errstr, "\n";
    unset($GLOBALS['x']);
});

$x = 1; var_dump($x + $y);
$x = 1; var_dump($x - $y);
$x = 1; var_dump($x * $y);
$x = 1; var_dump($x ** $y);
$x = 1; var_dump($x & $y);
$x = 1; var_dump($x | $y);
$x = 1; var_dump($x ^ $y);
$x = 1; var_dump($x << $y);
$x = 1; var_dump($x >> $y);
$x = 1;
try {
    var_dump($x / $y);
} catch (DivisionByZeroError $e) {
    echo $e->getMessage(), "\n";
}
$x = 1;
try {
    var_dump($x % $y);
} catch (DivisionByZeroError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Undefined variable $y
int(0)
Undefined variable $y
int(0)
Undefined variable $y
int(0)
Undefined variable $y
int(1)
Undefined variable $y
int(0)
Undefined variable $y
int(0)
Undefined variable $y
int(0)
Undefined variable $y
int(0)
Undefined variable $y
int(0)
Undefined variable $y
Division by zero
Undefined variable $y
Modulo by zero
