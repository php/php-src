--TEST--
#[\Override]: Constants - anonymous class overrides parent class
--FILE--
<?php

class Base {
    public const C = 'C';
}

new class () extends Base {
    #[\Override]
    public const C = 'Changed';
};

echo "Done";

?>
--EXPECT--
Done
