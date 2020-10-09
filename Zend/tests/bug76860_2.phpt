--TEST--
Bug #76860 (Missed "Accessing static property as non static" warning)
--FILE--
<?php
class A {
    private static $a = "a";
    private static $b = "b";
    private static $c = "c";
    public function __construct() {
    var_dump($this->a, $this->b, $this->c);
    }
}
class B extends A {
    private static $a = "a1";
    protected static $b = "b1";
    public static $c = "c1";
}
new B;
?>
--EXPECTF--
Notice: Accessing static property B::$a as non static in %sbug76860_2.php on line 7

Warning: Undefined property: B::$a in %s on line %d

Notice: Accessing static property B::$b as non static in %sbug76860_2.php on line 7

Warning: Undefined property: B::$b in %s on line %d

Notice: Accessing static property B::$c as non static in %sbug76860_2.php on line 7

Warning: Undefined property: B::$c in %s on line %d
NULL
NULL
NULL
