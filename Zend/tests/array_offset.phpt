--TEST--
Ensure "undefined offset" notice formats message corectly when undefined key is negative
--FILE--
<?php

[][-1];
[][-1.1];
(new ArrayObject)[-1];
(new ArrayObject)[-1.1];

echo "Done\n";
?>
--EXPECTF--
Notice: Undefined offset: -1 in %s on line 3

Notice: Undefined offset: -1 in %s on line 4

Notice: Undefined offset: -1 in %s on line 5

Notice: Undefined offset: -1 in %s on line 6
Done
