--TEST--
ZE2 __get() signature check
--FILE--
<?php
class Test {
    function __get($x,$y) {
    }
}

?>
--EXPECTF--
Fatal error: Method Test::__get() must take exactly 1 argument in %s__set__get_002.php on line %d
