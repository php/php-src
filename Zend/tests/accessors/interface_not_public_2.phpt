--TEST--
Cannot use non-public accessor in interface (whole property)
--FILE--
<?php

interface I {
    protected $prop { get; set; }
}

?>
--EXPECTF--
Fatal error: Accessor in interface cannot be protected or private in %s on line %d
