--TEST--
Variable with boolean name
--FILE--
<?php

${true} = 42;
var_dump(${true});
var_dump(${'1'});

?>
--EXPECT--
int(42)
int(42)
