--TEST--
Flexible heredoc syntax error 11: show erroneous line in error message (variable interpolation)
--FILE--
<?php

echo <<<END
 a
$a
 END;

?>
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation level of at least 1) in %s on line 5
