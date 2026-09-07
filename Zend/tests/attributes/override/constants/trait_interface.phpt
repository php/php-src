--TEST--
#[\Override]: Constants - on a trait, overrides interface
--FILE--
<?php

trait DemoTrait {
    #[\Override]
    public const C = 'Changed';
}

interface IFace {
    public const C = 'C';
}

class UsesTrait implements IFace {
    use DemoTrait;
}

echo "Done";

?>
--EXPECT--
Done
