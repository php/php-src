--TEST--
#[\Override]: enum without interface.
--FILE--
<?php

enum E {
    case One;
    case Two;

    #[\Override]
    public function e(): void {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: E::e() has #[\Override] attribute, but no matching parent method exists in %s on line %d
