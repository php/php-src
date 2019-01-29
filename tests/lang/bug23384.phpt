--TEST--
Bug #23384 (use of class constants in statics)
--FILE--
<?php
define('TEN', 10);
class Foo {
    const HUN = 100;
    static function test($x = Foo::HUN) {
        static $arr2 = array(TEN => 'ten');
        static $arr = array(Foo::HUN => 'ten');

        print_r($arr);
        print_r($arr2);
        print_r($x);
    }
}

Foo::test();
echo Foo::HUN."\n";
?>
--EXPECT--
Array
(
    [100] => ten
)
Array
(
    [10] => ten
)
100100
