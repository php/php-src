--TEST--
ZE2 An interface method cannot be private
--FILE--
<?php

interface if_a {
    abstract private function err();
}

?>
--EXPECTF--
Fatal error: Interface method if_a::err() must have public visibility in %s on line %d
