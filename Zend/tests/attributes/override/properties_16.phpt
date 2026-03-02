--TEST--
#[\Override]: Properties: Invalid anonymous class.
--FILE--
<?php

interface I {
    public mixed $i { get; }
}

new class () implements I {
    public mixed $i;

    #[\Override]
    public mixed $c;
};

echo "Done";

?>
--EXPECTF--
Fatal error: I@anonymous::$c has #[\Override] attribute, but no matching parent property exists in %s on line %d
