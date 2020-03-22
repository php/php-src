--TEST--
ZE2 __set() signature check
--FILE--
<?php
class Test {
    function __set() {
    }
}

?>
--EXPECTF--
Fatal error: Method Test::__set() must take exactly 2 arguments in %s__set__get_003.php on line %d
