--TEST--
Ensure "undefined offset" notice formats message correctly when undefined key is negative
--FILE--
<?php

[][-1];
[][-1.1];
(new ArrayObject)[-1];
(new ArrayObject)[-1.1];

echo "Done\n";
?>
--EXPECTF--
Warning: Undefined array key -1 in %s on line %d

Deprecated: Implicit conversion from float -1.1 to int loses precision in %s on line %d

Warning: Undefined array key -1 in %s on line %d

Warning: Undefined array key -1 in %s on line %d

Deprecated: Implicit conversion from float -1.1 to int loses precision in %s on line %d

Warning: Undefined array key -1 in %s on line %d
Done
