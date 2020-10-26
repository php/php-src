--TEST--
ZE2 An interface cannot have properties
--FILE--
<?php

interface if_a {
    public $member;
}
?>
--EXPECTF--
Fatal error: Interface if_a cannot have properties in %s on line %d
