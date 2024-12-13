--TEST--
Allow defining Closures in class constants.
--FILE--
<?php

class C {
    const Closure = static function () {
        echo "called", PHP_EOL;
    };
}

var_dump(C::Closure);
(C::Closure)();

?>
--EXPECTF--
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(4)
}
called
