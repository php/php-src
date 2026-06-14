--TEST--
#[\Override]: Properties: Redeclared trait property with interface.
--FILE--
<?php

interface I {
    public mixed $i { get; }
}

trait T {
    public mixed $i;
}

class C implements I {
    use T;

    #[\Override]
    public mixed $i;
}

echo "Done";

?>
--EXPECT--
Done
