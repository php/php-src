--TEST--
Cannot declare final accessor in interface
--FILE--
<?php
interface I {
    public $prop { final get; }
}
?>
--EXPECTF--
Fatal error: Accessor cannot be both abstract and final in %s on line %d
