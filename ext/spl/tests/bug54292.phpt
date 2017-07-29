--TEST--
Bug #54292 (Wrong parameter causes crash in SplFileObject::__construct())
--FILE--
<?php

try {
	new SplFileObject('foo', array());
} catch (TypeError $e) {
	var_dump($e->getMessage());
}

?>
--EXPECTF--
string(125) "SplFileObject::__construct() expects parameter 2 to be string (or integer, float, boolean or convertible object), array given"
