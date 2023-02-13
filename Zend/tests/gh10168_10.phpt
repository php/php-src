--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign typed prop
--FILE--
<?php

class Test {
    static ?Test $a = null;
    function __construct() {
        if(self::$a == null){
            self::$a = &$this;
        }
    }
    function __destruct(){
        self::$a = null;
    }
}
new Test;
new Test;

?>
--EXPECT--