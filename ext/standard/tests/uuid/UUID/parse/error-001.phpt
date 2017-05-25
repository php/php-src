--TEST--
UUID::parse throws ArgumentCountError if no arguments are passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
	UUID::parse();
}
catch (ArgumentCountError $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
UUID::parse() expects exactly 1 parameter, 0 given
