--TEST--
#[\Override]: Properties: Static property.
--FILE--
<?php

class P {
    public static mixed $p;
}

class C extends P {
    #[\Override]
    public static mixed $p;
}

echo "Done";

?>
--EXPECT--
Done
