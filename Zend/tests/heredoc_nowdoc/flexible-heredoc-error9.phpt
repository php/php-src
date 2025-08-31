--TEST--
Flexible heredoc syntax error 9: unindented variable interpolation
--FILE--
<?php

$var = 'Bar';
var_dump(<<<TEST
  Foo
$var
  TEST);

?>
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation level of at least 2) in %s on line %d
