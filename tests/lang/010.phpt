--TEST--
Testing function parameter passing with a return value
--FILE--
<?php
function test ($b) {
    $b++;
    return($b);
}
$a = test(1);
echo $a;
?>
--EXPECT--
2
