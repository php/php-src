--TEST--
#[\Override]: Static method.
--FILE--
<?php

class P {
    public static function p(): void {}
}

class C extends P {
    #[\Override]
    public static function p(): void {}
}

echo "Done";

?>
--EXPECT--
Done
