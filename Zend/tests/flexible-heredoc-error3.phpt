--TEST--
Flexible heredoc syntax error 3: mixing spaces and tabs in ending marker
--FILE--
<?php

try {
	echo <<<END
		 a
		 b
		 c
		 END;
} catch (ParseError $pe) {
	var_dump($e->getMessage());
}
--EXPECTF--
Parse error: Invalid indentation - tabs and spaces cannot be mixed in %s on line %d