--TEST--
#[\Override]: Properties: On trait.
--FILE--
<?php

trait T {
    #[\Override]
    public mixed $t;
}

echo "Done";

?>
--EXPECT--
Done
