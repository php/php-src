--TEST--
Flexible heredoc syntax error 12: show erroneous line in error message (mixed indentation)
--FILE--
<?php

echo <<<END
 a
	b
 END;

?>
--EXPECTF--
Parse error: Invalid indentation - tabs and spaces cannot be mixed in %s on line 5
