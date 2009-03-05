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
Notice: Use of undefined constant FOO - assumed 'FOO' in %s on line %d
