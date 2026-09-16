--TEST--
#[\Override]: Constants - trait constant redeclared, overrides interface
--FILE--
<?php

interface IFace {
    public const I = 'I';
}

trait DemoTrait {
    public const I = 'I';
}

class UsesTrait implements IFace {
    use DemoTrait;

    #[\Override]
    public const I = 'I';
}

echo "Done";

?>
--EXPECT--
Done
