--TEST--
Cannot declare final property in interface
--FILE--
<?php
interface I {
    final public $prop { get; set; }
}
?>
--EXPECTF--
Fatal error: Property in interface cannot be final in %s on line %d
