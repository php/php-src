--TEST--
Allow defining Closures passed as constructor arguments in const expressions.
--FILE--
<?php

class Dummy {
  public function __construct(
      public Closure $c,
  ) {}
}

const Closure = new Dummy(static function () {
  echo "called", PHP_EOL;
});

var_dump(Closure);

(Closure->c)();

?>
--EXPECTF--
object(Dummy)#%d (1) {
  ["c"]=>
  object(Closure)#%d (3) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(9)
  }
}
called
