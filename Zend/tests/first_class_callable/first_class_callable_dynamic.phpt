--TEST--
Acquire callable through various dynamic constructs
--FILE--
<?php

class A {
    public static function b($x) {
        return $x;
    }

    public function c($x) {
        return $x;
    }
}

$name = 'strlen';
$fn = $name(...);
var_dump($fn('x'));

$name = ['A', 'b'];
$fn = $name(...);
var_dump($fn(2));

$name = [new A, 'c'];
$fn = $name(...);
var_dump($fn(3));

$name1 = 'A';
$name2 = 'b';
$fn = $name1::$name2(...);
var_dump($fn(4));

$name2 = 'c';
$fn = (new A)->$name2(...);
var_dump($fn(5));

$fn = [A::class, 'b'](...);
var_dump($fn(6));

$o = new stdClass;
$o->prop = A::b(...);
($o->prop)(7);

$nam

?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
