--TEST--
Break out of while loop that is followed by a return statement and inside a foreach loop
--FILE--
<?php

$a = [42];
foreach ($a as $b) {
    while (1) {
        break 2;
    }
    return;
}
?>
===DONE===
--EXPECT--
===DONE===
