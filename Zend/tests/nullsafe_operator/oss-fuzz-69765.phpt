--TEST--
OSS-Fuzz #69765: yield reference to nullsafe chain
--FILE--
<?php
function &test($object) {
    yield $object->y?->y;
}
?>
--EXPECTF--
Fatal error: Cannot take reference of a nullsafe chain in %s on line %d
