--TEST--
array_key_exists() on known empty array
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
function helper(&$var) {
    var_dump($var);
}
class ExampleArrayKeyExists {
    const EMPTY_ARRAY = [];
    public static function test(int $x, array $arr) {
        $y = array_key_exists($x, self::EMPTY_ARRAY);
        $v2 = array_key_exists($undef, self::EMPTY_ARRAY);
        $z = array_key_exists($x, []);
        $z1 = array_key_exists($x, [1 => true]);
        $z2 = array_key_exists($x, [2 => true]);
        $w = array_key_exists('literal', self::EMPTY_ARRAY);
        echo helper($y);
        echo helper($z);
        echo helper($w);
        echo helper($z1);
        echo helper($z2);
        $unusedVar = array_key_exists('unused', $arr);
        if (array_key_exists(printf("Should get called\n"), self::EMPTY_ARRAY)) {
            echo "Impossible\n";
        }
        $v = array_key_exists($arr, self::EMPTY_ARRAY);
    }
}
try {
    ExampleArrayKeyExists::test(1,[2]);
} catch (TypeError $e) {
    printf("%s at line %d\n", $e->getMessage(), $e->getLine());
}
?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line 10
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
Should get called
Illegal offset type at line 24
