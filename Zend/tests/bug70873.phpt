--TEST--
Bug #70873 (Regression on private static properties access)
--FILE--
<?php

class A {
    private static $x = 1;
}

class B extends A {
    function bar() {
        var_dump(self::$x);
    }
};

class C extends A {
    function bar() {
        var_dump(A::$x);
    }
};


$a = new B;
$a->bar();

$b = new C;
$b->bar();
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access private property B::$x in %sbug70873.php:%d
Stack trace:
#0 %sbug70873.php(%d): B->bar()
#1 {main}
  thrown in %sbug70873.php on line %d
