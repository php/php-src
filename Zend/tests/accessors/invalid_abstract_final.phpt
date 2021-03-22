--TEST--
Accessor cannot be both abstract and final
--FILE--
<?php

class Test {
    public $prop { abstract final get; }
}

?>
--EXPECTF--
Fatal error: Cannot use the final modifier on an abstract class member in %s on line %d
