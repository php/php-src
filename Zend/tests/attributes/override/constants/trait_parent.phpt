--TEST--
#[\Override]: Constants - on a trait, overrides parent class
--FILE--
<?php

trait DemoTrait {
    #[\Override]
    public const C = 'Changed';
}

class Base {
    public const C = 'C';
}

class UsesTrait extends Base {
    use DemoTrait;
}

echo "Done";

?>
--EXPECT--
Done
