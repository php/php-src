--TEST--
UUID::v5 throws ArgumentCountError if only one argument is passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
	UUID::v5(UUID::Nil());
}
catch (ArgumentCountError $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
UUID::v5() expects exactly 2 parameters, 1 given
