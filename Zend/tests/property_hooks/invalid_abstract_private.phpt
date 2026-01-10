--TEST--
Property hook cannot be both abstract and private
--FILE--
<?php

class Test {
    private abstract $prop { get; }
}

?>
--EXPECTF--
Fatal error: Property hook cannot be both abstract and private in %s on line %d
