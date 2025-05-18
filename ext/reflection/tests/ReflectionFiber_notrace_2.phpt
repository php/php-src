--TEST--
ReflectionFiber should not segfault when inspecting fibers where the previous stack frame is a native function
--FILE--
<?php

namespace test;

$f = new \Fiber(fn() => call_user_func(["Fiber", "suspend"]));
$f->start();

$reflection = new \ReflectionFiber($f);

var_dump($reflection->getExecutingFile());
var_dump($reflection->getExecutingLine());
var_dump($reflection->getTrace());

?>
--EXPECTF--
string(%d) "%sReflectionFiber_notrace_2.php"
int(5)
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(5)
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
  [1]=>
  array(2) {
    ["function"]=>
    string(%d) "{closure:%s:%d}"
    ["args"]=>
    array(0) {
    }
  }
}
