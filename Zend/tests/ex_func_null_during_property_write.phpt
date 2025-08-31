--TEST--
EX(func) can be null during write_property in an edge case
--FILE--
<?php
class a {
    public static $i = 0;
    function __destruct() {
        if (self::$i++ != 0) throw new Exception;
        $b = new a;
        echo $b;
    }
}
new a;
?>
--EXPECTF--
Fatal error: Uncaught Error: Object of class a could not be converted to string in %s:%d
Stack trace:
#0 %s(%d): a->__destruct()
#1 {main}

Next Exception in %s:%d
Stack trace:
#0 %s(%d): a->__destruct()
#1 {main}
  thrown in %s on line %d
