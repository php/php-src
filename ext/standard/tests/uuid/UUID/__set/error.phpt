--TEST--
UUID::__set invocation leads to Error
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

try {
	UUID::Nil()->dynamic_property = 'value';
}
catch (Error $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
Cannot set dynamic properties on immutable PHP\Std\UUID object
