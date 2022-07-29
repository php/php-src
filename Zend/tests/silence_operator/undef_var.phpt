--TEST--
Silence undef variable with @ operator
--FILE--
<?php

$var = @$non_existent;

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
