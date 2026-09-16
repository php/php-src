--TEST--
Allow defining Closures in const expressions.
--FILE--
<?php

const Closure = static function () {
    echo "called", PHP_EOL;
};

var_dump(Closure);
(Closure)();

?>
--EXPECTF--
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(3)
}
called
