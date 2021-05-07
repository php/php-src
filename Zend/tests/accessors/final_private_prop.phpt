--TEST--
Property cannot be both final and private
--FILE--
<?php

class Test {
    final private $prop;
}

?>
--EXPECTF--
Fatal error: Property cannot be both final and private in %s on line %d
