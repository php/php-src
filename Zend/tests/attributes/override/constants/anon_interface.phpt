--TEST--
#[\Override]: Constants - anonymous class overrides interface
--FILE--
<?php

interface IFace {
    public const I = 'I';
}

new class () implements IFace {
    #[\Override]
    public const I = 'Changed';
};

echo "Done";

?>
--EXPECT--
Done
