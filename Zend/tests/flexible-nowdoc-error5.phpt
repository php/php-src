--TEST--
Flexible nowdoc syntax error 5: mixing spaces and tabs in ending marker for 0 length body
--FILE--
<?php

echo <<<'END'
	 END;

?>
--EXPECTF--
Parse error: Invalid indentation - tabs and spaces cannot be mixed in %s on line %d
