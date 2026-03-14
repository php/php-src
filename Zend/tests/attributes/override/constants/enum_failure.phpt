--TEST--
#[\Override]: Constants - no interface for enum
--FILE--
<?php

enum Demo {
    #[\Override]
    public const C = 'C';
}

echo "Done";

?>
--EXPECTF--
Fatal error: Demo::C has #[\Override] attribute, but no matching parent constant exists in %s on line %d
