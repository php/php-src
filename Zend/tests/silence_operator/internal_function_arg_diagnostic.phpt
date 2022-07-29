--TEST--
Silencing an internal function argument emitting a diagnostic
--FILE--
<?php

var_dump(@$non_existent);

echo "Done\n";
?>
--EXPECT--
NULL
Done
