--TEST--
Bug #73156 (segfault on undefined function)
--FILE--
<?php
class A {
    public function __call($name, $args) {
        eval('$args = array(); var_dump(debug_backtrace());');
    }
}

$a = new A();

$a->test("test");
?>
--EXPECTF--
array(2) {
  [0]=>
  array(3) {
    ["file"]=>
    string(%d) "%sbug73156.php"
    ["line"]=>
    int(4)
    ["function"]=>
    string(4) "eval"
  }
  [1]=>
  array(7) {
    ["file"]=>
    string(%d) "%sbug73156.php"
    ["line"]=>
    int(10)
    ["function"]=>
    string(6) "__call"
    ["class"]=>
    string(1) "A"
    ["object"]=>
    object(A)#%d (0) {
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(2) {
      [0]=>
      string(4) "test"
      [1]=>
      array(0) {
      }
    }
  }
}
