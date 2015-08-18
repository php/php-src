--TEST--
Variable with integer name
--FILE--
<?php

${10} = 42;
var_dump(${10});

?>
--EXPECT--
int(42)
