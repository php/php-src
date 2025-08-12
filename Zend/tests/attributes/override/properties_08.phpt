--TEST--
#[\Override]: Properties: On used trait with interface property.
--FILE--
<?php

trait T {
    #[\Override]
    public mixed $i;
}

interface I {
    public mixed $i { get; }
}

class Foo implements I {
    use T;
}

echo "Done";

?>
--EXPECT--
Done
