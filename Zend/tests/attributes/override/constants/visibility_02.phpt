--TEST--
#[\Override]: Constants - private constant not overridden (by private constant)
--FILE--
<?php

class Base {
    private const C = 'C';
}

class Child extends Base {
    #[\Override]
    private const C = 'Changed';
}

echo "Done";

?>
--EXPECTF--
Fatal error: Child::C has #[\Override] attribute, but no matching parent constant exists in %s on line %d
