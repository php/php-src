--TEST--
UUID::fromBinary throws ArgumentCountError if no arguments are passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
	UUID::fromBinary();
}
catch (ArgumentCountError $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
UUID::fromBinary() expects exactly 1 parameter, 0 given
