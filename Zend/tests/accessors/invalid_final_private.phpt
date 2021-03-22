--TEST--
Accessor cannot be both final and private
--FILE--
<?php

class Test {
    private $prop { final get; }
}

?>
--EXPECTF--
Fatal error: Accessor cannot be both final and private in %s on line %d
