--TEST--
#[\Override]: enum with matching interface.
--FILE--
<?php

interface I {
    public function e(): void;
}

enum E implements I {
    case One;
    case Two;

    #[\Override]
    public function e(): void {}
}

echo "Done";

?>
--EXPECT--
Done
