--TEST--
GH-20672: Incorrect property_info sizing for locally shadowed trait properties
--CREDITS--
Jonne Ransijn (yyny)
--FILE--
<?php

trait T {
    public static $a;
    public static $b;
    public static $c;
}

class Base {
    public $x;
    public $y;
}

class Child extends Base {
    public static $a;
    public static $b;
    public static $c;
    public static $d;

    use T;
}

?>
--EXPECT--
