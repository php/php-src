--TEST--
Bug #44144 (spl_autoload_functions() should return object instance when appropriate)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
class Foo {
  public function nonstaticMethod() {}
}
$foo = new Foo;
spl_autoload_register(array($foo, 'nonstaticMethod'));
$funcs = spl_autoload_functions();
var_dump($funcs);
?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(Foo)#%d (0) {
    }
    [1]=>
    string(15) "nonstaticMethod"
  }
}
