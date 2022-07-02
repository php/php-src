--TEST--
Bug #76860 (Missed "Accessing static property as non static" warning)
--FILE--
<?php
class A {
    private   static $a = "a";
    protected static $b = "b";
    public    static $c = "c";
    public function __construct() {
    var_dump($this->a, $this->b, $this->c);
    }
}
class B extends A {
}
new B;
?>
--EXPECTF--
Notice: Accessing static property B::$a as non static in %sbug76860.php on line 7

Warning: Undefined property: B::$a in %s on line %d

Notice: Accessing static property B::$b as non static in %sbug76860.php on line 7

Warning: Undefined property: B::$b in %s on line %d

Notice: Accessing static property B::$c as non static in %sbug76860.php on line 7

Warning: Undefined property: B::$c in %s on line %d
NULL
NULL
NULL
