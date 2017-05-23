--TEST--
UUID::__set invocation leads to Error
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
	UUID::Nil()->dynamic_property = 'value';
}
catch (Error $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Cannot set dynamic properties on immutable UUID object
