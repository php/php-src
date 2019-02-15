--TEST--
Flexible heredoc syntax error 8: don't interpret \t as indentation
--FILE--
<?php

<<<end
\ta
	end);

?>
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation level of at least 1) in %s on line %d
