--TEST--
#[\Override]: Constants - trait constant redeclared, not overridden
--FILE--
<?php

trait DemoTrait {
    public const T = 'T';
}

class UsesTrait {
    use DemoTrait;

    #[\Override]
    public const T = 'T';
}

echo "Done";

?>
--EXPECTF--
Fatal error: UsesTrait::T has #[\Override] attribute, but no matching parent constant exists in %s on line %d
