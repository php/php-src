--TEST--
#[\Override]: Constants - protected constant is overridden (by public constant)
--FILE--
<?php

class Base {
    protected const C = 'C';
}

class Child extends Base {
    #[\Override]
    public const C = 'Changed';
}

echo "Done";

?>
--EXPECT--
Done
