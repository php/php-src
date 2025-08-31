--TEST--
ReflectionFiber should not segfault when inspecting fibers with no stack frames before suspend
--FILE--
<?php
$f = new Fiber(Fiber::suspend(...));
$f->start();

$reflection = new ReflectionFiber($f);

var_dump($reflection->getExecutingFile());
var_dump($reflection->getExecutingLine());
var_dump($reflection->getTrace());

?>
--EXPECTF--
NULL
NULL
array(1) {
  [0]=>
  array(4) {
    ["function"]=>
    string(7) "suspend"
    ["class"]=>
    string(5) "Fiber"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(0) {
    }
  }
}
