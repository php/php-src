--TEST--
Flexible heredoc syntax error 10: unindented variable interpolation (as first value)
--FILE--
<?php

$var = 'Bar';
var_dump(<<<TEST
$var
 TEST);

?>
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation level of at least 1) in %s on line %d
