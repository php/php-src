--TEST--
Incorrect cfg block marking for two arm match
--FILE--
<?php
$x = 2;
var_dump(match ($x) {
    2,2 => 'x',
    default => 'y',
});
?>
--EXPECT--
string(1) "x"
