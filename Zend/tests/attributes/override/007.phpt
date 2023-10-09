--TEST--
#[\Override]: On trait.
--FILE--
<?php

trait T {
    #[\Override]
    public function t(): void {}
}

echo "Done";

?>
--EXPECT--
Done
