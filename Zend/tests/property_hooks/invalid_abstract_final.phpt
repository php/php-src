--TEST--
Property hook cannot be both abstract and final
--FILE--
<?php

class Test {
    public abstract $prop { final get; }
}

?>
--EXPECTF--
Fatal error: Property hook cannot be both abstract and final in %s on line %d
