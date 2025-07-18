--TEST--
#[\Override]: Properties: Parent property is protected.
--FILE--
<?php

class P {
    protected mixed $p;
}

class C extends P {
    #[\Override]
    public mixed $p;
}

echo "Done";

?>
--EXPECT--
Done
