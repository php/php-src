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
Warning: Use of undefined constant FOO - assumed 'FOO' (this will throw an Error in a future version of PHP) in %s on line %d
