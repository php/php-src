--TEST--
#[\Override]: Constants - on a trait, no interface or parent class
--FILE--
<?php

trait DemoTrait {
    #[\Override]
    public const T = 'T';
}

class UsesTrait {
    use DemoTrait;
}

echo "Done";

?>
--EXPECTF--
Fatal error: UsesTrait::T has #[\Override] attribute, but no matching parent constant exists in %s on line %d
