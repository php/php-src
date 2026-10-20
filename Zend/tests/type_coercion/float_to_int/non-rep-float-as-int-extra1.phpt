--TEST--
Non rep float string to int conversions should not crash when modified
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    global $b;
    $b = null;
    echo $errstr, "\n";
});

$a = "1.0E+" . rand(40,42);
$b = &$a;
var_dump($b | 1);

?>
--EXPECTF--
Implicit conversion from float-string "1.0E+4%d" to int loses precision
int(%d)
