--TEST--
Trying to create an object from dereferencing uninitialized variable
--FILE--
<?php

error_reporting(E_ALL);

class foo {
    public $x;
    static public $y;

    public function a() {
        return $this->x;
    }

    static public function b() {
        return self::$y;
    }
}

$foo = new foo;
$h = $foo->a()[0]->a;
var_dump($h);

$h = foo::b()[1]->b;
var_dump($h);

?>
--EXPECTF--
Warning: Trying to access array offset on null in %s on line %d

Warning: Attempt to read property "a" on null in %s on line %d
NULL

Warning: Trying to access array offset on null in %s on line %d

Warning: Attempt to read property "b" on null in %s on line %d
NULL
