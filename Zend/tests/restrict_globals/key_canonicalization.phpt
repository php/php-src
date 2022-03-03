--TEST--
$GLOBALS should have canonicalized keys
--FILE--
<?php

${1} = 42;
var_dump($GLOBALS[1]);

?>
--EXPECT--
int(42)
