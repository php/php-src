--TEST--
Cannot declare final hook in interface
--FILE--
<?php
interface I {
    public $prop { final get; }
}
?>
--EXPECTF--
Fatal error: Property hook cannot be both abstract and final in %s on line %d
