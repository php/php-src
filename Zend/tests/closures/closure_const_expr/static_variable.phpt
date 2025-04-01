--TEST--
Closures in const expressions support static variables.
--FILE--
<?php

const Closure = static function () {
    static $x = [];
    static $i = 1;
    $i *= 2;
    $x[] = $i;
    var_dump($x);
};

var_dump(Closure);
(Closure)();
(Closure)();
(Closure)();
var_dump(Closure);

?>
--EXPECTF--
object(Closure)#%d (4) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(3)
  ["static"]=>
  array(2) {
    ["x"]=>
    array(0) {
    }
    ["i"]=>
    int(1)
  }
}
array(1) {
  [0]=>
  int(2)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(4)
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(4)
  [2]=>
  int(8)
}
object(Closure)#%d (4) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(3)
  ["static"]=>
  array(2) {
    ["x"]=>
    array(3) {
      [0]=>
      int(2)
      [1]=>
      int(4)
      [2]=>
      int(8)
    }
    ["i"]=>
    int(8)
  }
}
