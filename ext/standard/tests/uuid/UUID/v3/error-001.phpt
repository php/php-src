--TEST--
UUID::v3 throws ArgumentCountError if no arguments are passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
	UUID::v3();
}
catch (ArgumentCountError $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
UUID::v3() expects exactly 2 parameters, 0 given
