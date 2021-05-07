--TEST--
Cannot make accessor explicitly final in final property
--FILE--
<?php

class Test {
    final public $prop {
        final get;
        final set;
    }
}

?>
--EXPECTF--
Fatal error: Accessor on final property cannot be explicitly final in %s on line %d
