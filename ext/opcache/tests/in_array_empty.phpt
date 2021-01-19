--TEST--
in_array() on known empty array
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
function helper(&$var) {
    var_dump($var);
}
class ExampleInArray {
    const EMPTY_ARRAY = [];
    public static function test(int $x, array $arr) {
        $y = in_array($x, self::EMPTY_ARRAY);
        $y2 = in_array($x, self::EMPTY_ARRAY, true);
        $v2 = in_array($undef, self::EMPTY_ARRAY);
        $z = in_array($x, []);
        $w = in_array('literal', self::EMPTY_ARRAY);
        $z1 = in_array($x, [1]);
        $z2 = in_array($x, [2]);
        $z3 = in_array($x, [1], true);
        $z4 = in_array($x, [2], true);
        echo helper($y);
        echo helper($y2);
        echo helper($z);
        echo helper($w);
        echo "Results for non-empty arrays\n";
        echo helper($z1);
        echo helper($z2);
        echo helper($z3);
        echo helper($z4);
        $unusedVar = in_array('unused', $arr);
        if (in_array(printf("Should get called\n"), self::EMPTY_ARRAY)) {
            echo "Impossible\n";
        }
    }
}
ExampleInArray::test(1,[2]);
?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line 11
bool(false)
bool(false)
bool(false)
bool(false)
Results for non-empty arrays
bool(true)
bool(false)
bool(true)
bool(false)
Should get called