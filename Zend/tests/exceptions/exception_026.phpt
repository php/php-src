--TEST--
Nested exceptions in destructors
--FILE--
<?php
class A {
    static $max=0;
    function __destruct() {
        if (self::$max--<0)
            X;  
        $a = new A;
        Y;      
    }
}
new A;
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "Y" in %s:8
Stack trace:
#0 %s(11): A->__destruct()
#1 {main}

Next Error: Undefined constant "X" in %s:6
Stack trace:
#0 %s(11): A->__destruct()
#1 {main}
  thrown in %s on line 6
