--TEST--
Test possible declarations for expr&stmt options
--FILE--
<?php
function expr() => 123; // returns 123

var_dump(expr());

?>
--EXPECT--
int(123)