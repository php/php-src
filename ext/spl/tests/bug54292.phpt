--TEST--
Bug #54292 (Wrong parameter causes crash in SplFileObject::__construct())
--FILE--
<?php

try {
	new SplFileObject('foo', array());
} catch (Exception $e) {
	var_dump($e->getMessage());
}

?>
--EXPECTF--
string(74) "SplFileObject::__construct() expects parameter 2 to be string, array given"
