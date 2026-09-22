--TEST--
Non rep float string to int conversions should not crash when modified
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    global $ary;
    $ary = null;
    echo $errstr, "\n";
});

$ary = [rand()];
var_dump(isset($ary[1.0E+42]));

?>
--EXPECT--
The float 1.0E+42 is not representable as an int, cast occurred
bool(false)
