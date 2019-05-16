--TEST--
Flexible heredoc syntax 2: mixing spaces and tabs in body
--FILE--
<?php

echo <<<END
    	a
    	b
    	c
     END;

?>
--EXPECTF--
Parse error: Invalid indentation - tabs and spaces cannot be mixed in %s on line %d
