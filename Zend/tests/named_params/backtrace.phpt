--TEST--
Extra named params in backtraces
--FILE--
<?php

function test($a, ...$rest) {
    var_dump(debug_backtrace());
    debug_print_backtrace();
    throw new Exception("Test");
}

try {
    test(1, 2, x: 3, y: 4);
} catch (Exception $e) {
    var_dump($e->getTrace());
    echo $e, "\n";
}

?>
--EXPECTF--
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(10)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(4) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      ["x"]=>
      int(3)
      ["y"]=>
      int(4)
    }
  }
}
#0  test(1, 2, x: 3, y: 4) called at [%s:10]
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(10)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(4) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      ["x"]=>
      int(3)
      ["y"]=>
      int(4)
    }
  }
}
Exception: Test in %s:%d
Stack trace:
#0 %s(%d): test(1, 2, x: 3, y: 4)
#1 {main}
