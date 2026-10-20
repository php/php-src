--TEST--
#[\Override]: Properties: Valid anonymous class.
--FILE--
<?php

interface I {
    public mixed $i { get; }
}

new class () implements I {
    #[\Override]
    public mixed $i;
};

echo "Done";

?>
--EXPECT--
Done
