--TEST--
Bug #47572 (zval_update_constant_ex: Segmentation fault)
--FILE--
<?php

class Foo {
  public static $bar = array(
    FOO => "bar"
    );

}

$foo = new Foo();

?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "FOO" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
