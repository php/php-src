--TEST--
GH-11121: Segfault when using ReflectionFiber
--FILE--
<?php

function f() {
    Fiber::suspend();
}

function g() {
    (new Fiber(function() {
        global $f;
        var_dump((new ReflectionFiber($f))->getTrace());
    }))->start();
}

$f = new Fiber(function() { f(); g(); });
$f->start();
$f->resume();

?>
--EXPECTF--
array(3) {
  [0]=>
  array(7) {
    ["file"]=>
    string(%d) "%sReflectionFiber_bug_gh11121_2.php"
    ["line"]=>
    int(11)
    ["function"]=>
    string(5) "start"
    ["class"]=>
    string(5) "Fiber"
    ["object"]=>
    object(Fiber)#3 (0) {
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
  [1]=>
  array(4) {
    ["file"]=>
    string(%d) "%sReflectionFiber_bug_gh11121_2.php"
    ["line"]=>
    int(14)
    ["function"]=>
    string(1) "g"
    ["args"]=>
    array(0) {
    }
  }
  [2]=>
  array(2) {
    ["function"]=>
    string(9) "{closure}"
    ["args"]=>
    array(0) {
    }
  }
}
