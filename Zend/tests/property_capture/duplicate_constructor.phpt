--TEST--
Cannot use both property capture and custom constructor
--FILE--
<?php
$a = 42;
$foo = new class use ($a) {
    public function __construct() {}
}
?>
--EXPECTF--
Fatal error: Cannot declare custom constructor for anonymous class with captured properties in %s on line %d
