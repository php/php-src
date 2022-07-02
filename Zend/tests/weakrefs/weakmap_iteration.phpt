--TEST--
WeakMap iteration
--FILE--
<?php

$map = new WeakMap;

echo "\nEmpty loop:\n";
foreach ($map as $key => $value) {
    var_dump($key, $value);
}

echo "\nSimple loop:\n";
$obj0 = new stdClass;
$obj1 = new stdClass;
$obj2 = new stdClass;
$map[$obj0] = 0;
$map[$obj1] = 1;
$map[$obj2] = 2;
foreach ($map as $key => $value) {
    var_dump($key, $value);
}

echo "\nObject removed during loop:\n";
foreach ($map as $key => $value) {
    if (isset($obj1) && $key === $obj1) unset($obj1);
    var_dump($key, $value);
}

echo "\nBy reference iteration:\n";
foreach ($map as $key => &$value) {
    $value++;
}
var_dump($map);

?>
--EXPECT--
Empty loop:

Simple loop:
object(stdClass)#2 (0) {
}
int(0)
object(stdClass)#3 (0) {
}
int(1)
object(stdClass)#4 (0) {
}
int(2)

Object removed during loop:
object(stdClass)#2 (0) {
}
int(0)
object(stdClass)#3 (0) {
}
int(1)
object(stdClass)#4 (0) {
}
int(2)

By reference iteration:
object(WeakMap)#1 (2) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#2 (0) {
    }
    ["value"]=>
    &int(1)
  }
  [1]=>
  array(2) {
    ["key"]=>
    object(stdClass)#4 (0) {
    }
    ["value"]=>
    &int(3)
  }
}
