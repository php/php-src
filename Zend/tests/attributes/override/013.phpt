--TEST--
#[\Override]: Parent method is protected (2).
--FILE--
<?php

class P {
    protected function p(): void {}
}

class C extends P {
    #[\Override]
    protected function p(): void {}
}

echo "Done";

?>
--EXPECT--
Done
