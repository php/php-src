--TEST--
#[\Override]: Constants - anonymous class, no interface or parent class
--FILE--
<?php

new class () {
    #[\Override]
    public const C = 'C';
};

echo "Done";

?>
--EXPECTF--
Fatal error: class@anonymous::C has #[\Override] attribute, but no matching parent constant exists in %s on line %d
