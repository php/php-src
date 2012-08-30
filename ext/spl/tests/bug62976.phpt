--TEST--
Bug #62976 (Notice: could not be converted to int when comparing some builtin classes)
--FILE--
<?php

var_dump(new stdClass() == new ArrayObject());
var_dump(new stdClass() == new Exception());
var_dump(new ArrayObject() == new ArrayObject());
var_dump(new ArrayObject(array(1, 2)) == new ArrayObject());
?>
--EXPECTF--
bool(false)
bool(false)
bool(true)
bool(false)
