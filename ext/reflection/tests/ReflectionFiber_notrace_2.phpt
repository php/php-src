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
array(3) {
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
  [1]=>
  array(4) {
    ["file"]=>
    string(%d) "%sReflectionFiber_notrace_2.php"
    ["line"]=>
    int(5)
    ["function"]=>
    string(14) "call_user_func"
    ["args"]=>
    array(1) {
      [0]=>
      array(2) {
        [0]=>
        string(5) "Fiber"
        [1]=>
        string(7) "suspend"
      }
    }
  }
  [2]=>
  array(2) {
    ["function"]=>
    string(%d) "test\{closure:%s:%d}"
    ["args"]=>
    array(0) {
    }
  }
}
