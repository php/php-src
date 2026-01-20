--TEST--
#[\Override]: Constants - basic
--FILE--
<?php

interface I {
    public const I = 'I';
}

interface II extends I {
    #[\Override]
    public const I = 'I';
}

class P {
    public const C1 = 'C1';
    public const C2 = 'C2';
}

class PP extends P {
    #[\Override]
    public const C1 = 'C1';
    public const C2 = 'C2';
}

class C extends PP implements I {
    #[\Override]
    public const I = 'I';
    #[\Override]
    public const C1 = 'C1';
    public const C2 = 'C2';
    public const C = 'C';
}

echo "Done";

?>
--EXPECT--
Done
