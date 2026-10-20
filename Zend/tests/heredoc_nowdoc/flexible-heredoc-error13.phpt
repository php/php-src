--TEST--
Flexible heredoc syntax error 12: show erroneous line in error message (lacking indentation)
--FILE--
<?php

echo <<<END
 a
b
 END;

?>
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation level of at least 1) in %s on line 5
