--TEST--
__HALT_COMPILER() basic test
--FILE--
<?php

print "yo!\n";

__halt_compiler();

none of this should be displayed!

?>
--EXPECT--
yo!
