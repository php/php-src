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
Fatal error: class@anonymous%0%s:3$%x::C has #[\Override] attribute, but no matching parent constant exists in %s on line %d
