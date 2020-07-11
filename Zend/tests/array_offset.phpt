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
Notice: Undefined array key -1 in %s on line %d

Notice: Undefined array key -1 in %s on line %d

Notice: Undefined array key -1 in %s on line 5

Notice: Undefined array key -1 in %s on line 6
Done
