--TEST--
#[\Override]: Properties: Parent property is protected, child property is public.
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
