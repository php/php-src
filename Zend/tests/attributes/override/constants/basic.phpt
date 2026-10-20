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
    public const C3 = 'C3';
    public const C4 = 'C4';
}

class PP extends P {
    #[\Override]
    public const C1 = 'C1';
    public const C2 = 'C2';
    #[\Override]
    public const C3 = 'C3';
}

class C extends PP implements I {
    #[\Override]
    public const I = 'I';
    #[\Override]
    public const C1 = 'C1';
    #[\Override]
    public const C2 = 'C2';
    public const C3 = 'C3';
    #[\Override]
    public const C4 = 'C4';
    public const C = 'C';
}

enum E implements I {
    #[\Override]
    public const I = 'I';
}

enum WithCase implements I {
    #[\Override]
    case I;
}

echo "Done";

?>
--EXPECT--
Done
