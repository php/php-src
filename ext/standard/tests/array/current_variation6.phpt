--TEST--
Test current() function : internal pointer maintenance at the end of array
--FILE--
<?php
$array = ["foo" => 1, "bar" => 2, "baz" => 3];
reset($array);
while ($cur = current($array)) {
    var_dump($cur);
    next($array);
}

unset($array["baz"]);
$array[] = 4;
var_dump(current($array));
?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
