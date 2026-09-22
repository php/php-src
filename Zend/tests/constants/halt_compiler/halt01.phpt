--TEST--
__HALT_COMPILER() basic test
--FILE--
<?php

print "yo!\n";

__HALT_COMPILER();

none of this should be displayed!
?>
--EXPECT--
yo!
