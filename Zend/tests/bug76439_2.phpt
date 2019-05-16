--TEST--
Bug #76439: Don't always strip leading whitespace from heredoc T_ENCAPSED_AND_WHITESPACE tokens (error case)
--FILE--
<?php

$foo = 1;

var_dump(<<<BAR
 $foo
$foo
 BAR);

?>
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation level of at least 1) in %s on line 7
