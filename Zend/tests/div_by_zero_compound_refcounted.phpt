--TEST--
Division by zero in compound assignment with refcounted operand
--FILE--
<?php
$h = "1";
$h .= "2";
try {
    $h /= 0;
} catch (DivisionByZeroError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($h);
?>
--EXPECT--
Division by zero
string(2) "12"
