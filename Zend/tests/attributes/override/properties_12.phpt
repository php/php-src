--TEST--
#[\Override]: Properties: Parent property is protected (2).
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
