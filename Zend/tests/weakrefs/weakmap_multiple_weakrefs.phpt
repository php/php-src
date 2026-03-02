--TEST--
Multiple WeakMaps / WeakRefs on the same object
--FILE--
<?php

$obj = new stdClass;

$ref = WeakReference::create($obj);
$map = new WeakMap;
$map[$obj] = 1;
$map2 = new WeakMap;
$map2[$obj] = 1;
$map3 = clone $map2;

var_dump($ref === WeakReference::create($obj));
var_dump($ref->get(), $map, $map2, $map3);
unset($obj);
var_dump($ref->get(), $map, $map2, $map3);
unset($ref, $map, $map2);

$obj = new stdClass;
$ref = WeakReference::create($obj);
$map = new WeakMap;
$map[$obj] = 1;
$map2 = new WeakMap;
$map2[$obj] = 1;
$map3 = clone $map2;

unset($ref, $map, $map2, $map3);
var_dump($obj);
unset($obj);

?>
--EXPECT--
bool(true)
object(stdClass)#1 (0) {
}
object(WeakMap)#3 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#1 (0) {
    }
    ["value"]=>
    int(1)
  }
}
object(WeakMap)#4 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#1 (0) {
    }
    ["value"]=>
    int(1)
  }
}
object(WeakMap)#5 (1) {
  [0]=>
  array(2) {
    ["key"]=>
    object(stdClass)#1 (0) {
    }
    ["value"]=>
    int(1)
  }
}
NULL
object(WeakMap)#3 (0) {
}
object(WeakMap)#4 (0) {
}
object(WeakMap)#5 (0) {
}
object(stdClass)#4 (0) {
}
