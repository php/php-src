--TEST--
Cannot use non-public property hook in interface (whole property)
--FILE--
<?php

interface I {
    protected $prop { get; set; }
}

?>
--EXPECTF--
Fatal error: Property in interface cannot be protected or private in %s on line %d
