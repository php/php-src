--TEST--
Cannot use parent type outside a class/trait: interface
--FILE--
<?php

interface T {
    function foo($x): parent;
}

?>
--EXPECTF--
Fatal error: Cannot use "parent" when current class scope has no parent in %s on line %d
