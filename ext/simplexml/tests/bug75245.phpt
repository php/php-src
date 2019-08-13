--TEST--
Bug #75245 Don't set content of elements with only whitespaces
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) die('skip simplexml not available');
?>
--FILE--
<?php
var_dump(simplexml_load_string('<test1><test2>    </test2><test3></test3></test1>'));
?>
===DONE===
--EXPECT--
object(SimpleXMLElement)#1 (2) {
  ["test2"]=>
  object(SimpleXMLElement)#2 (0) {
  }
  ["test3"]=>
  object(SimpleXMLElement)#3 (0) {
  }
}
===DONE===
