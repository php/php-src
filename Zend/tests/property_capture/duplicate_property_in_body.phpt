--TEST--
Error if captured property has same name as declared property
--FILE--
<?php
$a = 42;
$foo = new class use ($a) {
    private $a;
};
?>
--EXPECTF--
Fatal error: Captured property $a conflicts with existing property in %s on line %d
