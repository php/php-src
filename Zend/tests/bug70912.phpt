--TEST--
Bug #70912 Null ptr dereference instantiating class with invalid array property
--FILE--
<?php
new class {
    public $a = [][];
};
?>
--EXPECTF--
Fatal error: Cannot use [] for reading in %s on line %d
