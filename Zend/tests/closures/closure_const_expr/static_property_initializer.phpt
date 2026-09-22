--TEST--
Closure in static property initializer
--FILE--
<?php

class C {
    public static Closure $d = static function () {
        echo "called", PHP_EOL;
    };
}

var_dump(C::$d);
(C::$d)();


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
