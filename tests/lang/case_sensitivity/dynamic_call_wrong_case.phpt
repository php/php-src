--TEST--
Dynamic function call with wrong case fails
--FILE--
<?php
$fn = "STRLEN";
try {
    echo $fn("hello") . "\n";
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

$fn2 = "strlen";
echo $fn2("hello") . "\n";
?>
--EXPECT--
Call to undefined function STRLEN()
5
