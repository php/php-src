--TEST--
GH-15137: Unexpected null pointer in Zend/zend_smart_str.h
--EXTENSIONS--
dom
--FILE--
<?php
var_dump(new DOMText()->wholeText);
?>
--EXPECT--
string(0) ""

