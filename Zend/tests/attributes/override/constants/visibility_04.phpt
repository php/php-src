--TEST--
#[\Override]: Constants - protected constant is overridden (by protected constant)
--FILE--
<?php

class Base {
    protected const C = 'C';
}

class Child extends Base {
    #[\Override]
    protected const C = 'Changed';
}

echo "Done";

?>
--EXPECT--
Done
