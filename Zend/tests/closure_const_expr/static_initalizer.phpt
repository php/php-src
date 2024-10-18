--TEST--
Closure in static initializer
--FILE--
<?php

function foo() {
    static $closure = static function () {
        echo "called", PHP_EOL;
    };

    var_dump($closure);
    $closure();
}

foo();

?>
--EXPECTF--
object(Closure)#%d (3) {
  ["name"]=>
  string(17) "{closure:foo():4}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(4)
}
called
