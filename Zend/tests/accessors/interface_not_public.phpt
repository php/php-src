--TEST--
Cannot use non-public accessor in interface
--FILE--
<?php

interface I {
    public $prop { get; private set; }
}

?>
--EXPECTF--
Fatal error: Accessor in interface cannot be protected or private in %s on line %d
