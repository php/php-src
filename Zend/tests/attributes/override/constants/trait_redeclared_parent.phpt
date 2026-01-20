--TEST--
#[\Override]: Constants - trait constant redeclared, overrides parent class
--FILE--
<?php

class Base {
    public const I = 'I';
}

trait DemoTrait {
    public const I = 'I';
}

class UsesTrait extends Base {
    use DemoTrait;

    #[\Override]
    public const I = 'I';
}

echo "Done";

?>
--EXPECT--
Done
