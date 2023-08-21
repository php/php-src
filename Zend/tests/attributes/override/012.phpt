--TEST--
#[\Override]: Parent method is protected.
--FILE--
<?php

class P {
    protected function p(): void {}
}

class C extends P {
    #[\Override]
    public function p(): void {}
}

echo "Done";

?>
--EXPECT--
Done
