--TEST--
Closure in property initializer
--FILE--
<?php

class C {
    public Closure $d = static function () {
        echo "called", PHP_EOL;
    };
}

$c = new C();
var_dump($c->d);
($c->d)();


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
