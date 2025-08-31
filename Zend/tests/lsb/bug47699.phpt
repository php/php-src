--TEST--
Bug #47699 (autoload and late static binding)
--FILE--
<?php
class A {
    static function test($v='') {
        print_r(get_called_class());
    }
}
class B extends A {
}
B::test();
spl_autoload_register('B::test');
new X();
?>
--EXPECTF--
BB
Fatal error: Uncaught Error: Class "X" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug47699.php on line %d
