--TEST--
#[\Override]: Properties
--FILE--
<?php

interface I {
    public mixed $i { get; }
}

interface II extends I {
    #[\Override]
    public mixed $i { get; }
}

class P {
    public mixed $p1;
    public mixed $p2;
}

class PP extends P {
    #[\Override]
    public mixed $p1;
    public mixed $p2;
}

class C extends PP implements I {
    #[\Override]
    public mixed $i;
    #[\Override]
    public mixed $p1;
    public mixed $p2;
    public mixed $c;
}

echo "Done";

?>
--EXPECT--
Done
