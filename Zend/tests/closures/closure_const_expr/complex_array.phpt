--TEST--
Allow defining Closures wrapped in an array in const expressions.
--FILE--
<?php

const Closure = [static function () {
    echo "called", PHP_EOL;
}, static function () {
    echo "also called", PHP_EOL;
}];

var_dump(Closure);

foreach (Closure as $closure) {
    $closure();
}

?>
--EXPECTF--
array(2) {
  [0]=>
  object(Closure)#%d (3) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(3)
  }
  [1]=>
  object(Closure)#%d (3) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(5)
  }
}
called
also called
