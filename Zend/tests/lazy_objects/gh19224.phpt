--TEST--
GH-19224: Lazy ghosts may be partially initialized
--FILE--
<?php

class C {
    public int $x;
    public int $y;
}

$rc = new ReflectionClass(C::class);
$initializer = function ($obj, $prop) {
    if ($prop === null || $prop === 'x') {
        $obj->x = 1;
    }
    if ($prop === null || $prop === 'y') {
        $obj->y = 2;
    }
};

$obj = $rc->newLazyGhost($initializer, ReflectionClass::PARTIAL_INITIALIZATION);
var_dump($obj);
var_dump($obj->x);
var_dump($obj);
var_dump($obj->y);
var_dump($obj);

$obj = $rc->newLazyGhost($initializer, ReflectionClass::PARTIAL_INITIALIZATION);
foreach ($obj as $prop) {}
var_dump($obj);

// Object is realized when no specific prop is requested.
$obj = $rc->newLazyGhost(function () {}, ReflectionClass::PARTIAL_INITIALIZATION);
foreach ($obj as $prop) {}
var_dump($obj);

?>
--EXPECTF--
lazy ghost object(C)#%d (0) {
  ["x"]=>
  uninitialized(int)
  ["y"]=>
  uninitialized(int)
}
int(1)
lazy ghost object(C)#%d (1) {
  ["x"]=>
  int(1)
  ["y"]=>
  uninitialized(int)
}
int(2)
object(C)#%d (2) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
}
object(C)#%d (2) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
}
object(C)#%d (0) {
  ["x"]=>
  uninitialized(int)
  ["y"]=>
  uninitialized(int)
}
