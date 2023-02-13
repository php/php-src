--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign static prop ref
--FILE--
<?php

class Test {
    static $a = null;
    function __construct() {
        var_dump(self::$a = &$this);
    }
    function __destruct() {
        self::$a = null;
    }
}
new Test;
new Test;

?>
--EXPECT--
object(Test)#1 (0) {
}
NULL
