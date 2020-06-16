--TEST--
Bug #66609 (php crashes with __get() and ++ operator in some cases)
--FILE--
<?php
$bar = new Bar;
$foo = new Foo;
class Bar {
    public function __get($x) {
        global $foo;
        return $foo->foo;
    }
}
class Foo {
    public function __get($x) {
        global $bar;
        return $bar->bar;
    }
}
$foo->blah += 1; //crash
++$foo->blah;    //crash
$foo->blah++;    //crash
$foo->blah--;    //crash
--$foo->blah;    //crash
echo "okey";
?>
--EXPECTF--
Warning: Undefined property: Bar::$bar in %s on line %d
okey
