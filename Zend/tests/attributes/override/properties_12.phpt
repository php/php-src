--TEST--
#[\Override]: Properties: Parent property is protected, child property is protected.
--FILE--
<?php

class P {
    protected mixed $p;
}

class C extends P {
    #[\Override]
    protected mixed $p;
}

echo "Done";

?>
--EXPECT--
Done
