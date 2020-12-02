--TEST--
Silence throw with @ operator
--FILE--
<?php

$var = @throw new Exception();

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
