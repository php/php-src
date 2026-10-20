--TEST--
#[\Override]: Constants - on a trait, unused
--FILE--
<?php

trait Demo {
    #[\Override]
    public const T = 'T';
}

echo "Done";

?>
--EXPECT--
Done
