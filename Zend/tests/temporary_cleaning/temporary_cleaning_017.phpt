--TEST--
Live range & free on return & TMP var of RETURN opcode
--FILE--
<?php
class bar{
    public $y;
    function __destruct() {
        y;
    }
}
foreach(new bar as $y) {
    try {
        return new Exception;
    } catch(y) {
    }
}
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %stemporary_cleaning_017.php:5
Stack trace:
#0 %stemporary_cleaning_017.php(10): bar->__destruct()
#1 {main}
  thrown in %stemporary_cleaning_017.php on line 5
