--TEST--
Empty match subject with default "true"
--FILE--
<?php
var_dump(match {
    true => 'x',
    default => 'y',
});

var_dump(match {
    false => 'x',
    !false => 'y',
    default => 'z',
});

var_dump(match {
    1 > 2 => 'x',
    3 < 2 => 'y',
    default => 'z',
});
?>
--EXPECT--
string(1) "x"
string(1) "y"
string(1) "z"
